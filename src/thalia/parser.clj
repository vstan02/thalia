;; Copyright (C) 2024 Stan Vlad <vstan02@protonmail.com>
;;
;; This file is part of Thalia.
;;
;; Thalia is free software: you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as published by
;; the Free Software Foundation, either version 3 of the License, or
;; (at your option) any later version.
;;
;; This program is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
;; GNU General Public License for more details.
;;
;; You should have received a copy of the GNU General Public License
;; along with this program. If not, see <https://www.gnu.org/licenses/>.

(ns thalia.parser
  (:gen-class)
  (:require [thalia.token :as token]))

(def ^:private parse-expr nil)
(def ^:private parse-stmt nil)

(defn ^:private at-end? [tokens]
  (or (token/check (first tokens) #{:EOF}) (empty? tokens)))

(defn ^:private skip-until [types tokens]
  (drop-while #(not (token/check % types)) tokens))

(defn ^:private gen-parse-repeated [delims parse-item]
  (fn [tokens]
    (let [$res1 (parse-item tokens)]
      (loop [$nodes [(:ast $res1)]
             $errors (:errors $res1)
             $tokens (:rest $res1)]
        (if-not (token/check (first $tokens) delims)
          {:ast $nodes :errors $errors :rest $tokens}
          (let [$res2 (->> $tokens rest parse-item)]
            (recur (conj $nodes (:ast $res2))
                   (concat $errors (:errors $res2))
                   (:rest $res2))))))))

(defn ^:private parse-pattern [nname tokens rules]
  (loop [node {:type nname}
         errors []
         rules0 rules
         tokens0 tokens]
    (if (or (empty? rules0) (empty? tokens0))
      {:ast node :errors errors :rest tokens0}
      (let [rule (first rules0)
            tkn (first tokens0)]
        (if (some? (:values rule))
          (if (token/check tkn (:values rule))
            (recur (merge (when (:field rule) {(:field rule) tkn}) node)
                   errors
                   (rest rules0)
                   (rest tokens0))
            {:ast node
             :errors (conj errors {:type (:error rule) :token tkn})
             :rest tokens0})
          (let [res ((:function rule) tokens0)]
            (recur (merge {(:field rule) (:ast res)} node)
                   (concat errors (:errors res))
                   (rest rules0)
                   (:rest res))))))))

(defn ^:private parse-expr-binary [tokens types parse-operand]
  (let [res1 (parse-operand tokens)]
    (loop [node (:ast res1)
           errors (:errors res1)
           tokens0 (:rest res1)]
      (if-not (token/check (first tokens0) types)
        {:ast node :errors errors :rest tokens0}
        (let [res2 (parse-pattern
                    :EXPR-BINARY tokens0
                    [{:values types :error :EXPECT-BINARY-OPERATOR :field :operator}
                     {:function parse-operand :field :right}])]
          (recur (merge {:left node} (:ast res2))
                 (concat errors (:errors res2))
                 (:rest res2)))))))

(defn ^:private parse-each-values [tokens]
  (let [value1 (parse-expr tokens)
        tokens1 (:rest value1)]
    (if (token/check (first tokens1) #{:COLON})
      (let [value2 (->> tokens1 rest parse-expr)
            tokens2 (:rest value2)]
        (if (token/check (first tokens2) #{:COLON})
          (let [value3 (->> tokens2 rest parse-expr)]
            {:ast {:from (:ast value1) :to (:ast value3) :step (:ast value2)}
             :errors (concat (:errors value1) (:errors value2) (:errors value3))
             :rest (:rest value3)})
          {:ast {:from (:ast value1) :to (:ast value2)}
           :errors (concat (:errors value1) (:errors value2))
           :rest (:rest value2)}))
      {:ast {:to (:ast value1)} :errors (:errors value1) :rest tokens1})))

(defn ^:private parse-expr-primary [tokens]
  (case (->> tokens first :type)
    :LPAREN
    (parse-pattern
     :EXPR-GROUPING tokens
     [{:values #{:LPAREN} :error :EXPECT-LPAREN}
      {:function parse-expr :field :value}
      {:values #{:RPAREN} :error :EXPECT-RPAREN}])
    :ID
    (parse-pattern
     :EXPR-VARIABLE tokens
     [{:values #{:ID} :error :EXPECT-VARIABLE :field :token}])
    (parse-pattern
     :EXPR-LITERAL tokens
     [{:values #{:INT} :error :EXPECT-LITERAL :field :token}])))

(defn ^:private parse-expr-unary [tokens]
  (if-not (token/check (first tokens) #{:BANG :MINUS})
    (parse-expr-primary tokens)
    (parse-pattern
     :EXPR-UNARY tokens
     [{:values #{:MINUS :BANG} :error :EXPECT-UNARY-OPERATOR :field :operator}
      {:function parse-expr-unary :field :value}])))

(defn ^:private parse-expr-factor [tokens]
  (parse-expr-binary
   tokens
   #{:STAR :SLASH :PERCENT}
   parse-expr-unary))

(defn ^:private parse-expr-term [tokens]
  (parse-expr-binary
   tokens
   #{:PLUS :MINUS}
   parse-expr-factor))

(defn ^:private parse-expr-comp [tokens]
  (parse-expr-binary
   tokens
   #{:GREATER :LESS :GREATER-EQUAL :LESS-EQUAL}
   parse-expr-term))

(defn ^:private parse-expr-equal [tokens]
  (parse-expr-binary
   tokens
   #{:EQUAL-EQUAL :BANG-EQUAL}
   parse-expr-comp))

(defn ^:private parse-expr-logic-and [tokens]
  (parse-expr-binary
   tokens
   #{:AMPER-AMPER}
   parse-expr-equal))

(defn ^:private parse-expr-logic-or [tokens]
  (parse-expr-binary
   tokens
   #{:PIPE-PIPE}
   parse-expr-logic-and))

(defn ^:private parse-expr-assign [tokens]
  (let [res1 (parse-expr-logic-or tokens)]
    (if-not (token/check (->> res1 :rest first) #{:EQUAL})
      res1
      (let [res2 (->> res1 :rest rest parse-expr)]
        {:ast {:type :EXPR-ASSIGN
               :target (:ast res1)
               :value (:ast res2)}
         :errors (concat (:errors res1) (:errors res2))
         :rest (:rest res2)}))))

(defn ^:private parse-expr [tokens]
  (parse-expr-assign tokens))

(defn ^:private parse-stmt-expression [tokens]
  (parse-pattern
   :STMT-EXPRESSION tokens
   [{:function parse-expr :field :value}
    {:values #{:SEMI} :error :EXPECT-SEMI}]))

(defn ^:private parse-stmt-print [tokens]
  (->> [{:values #{:PRINT} :error :EXPECT-PRINT}
        (when-not (token/check (second tokens) #{:SEMI})
          {:function (gen-parse-repeated #{:COMMA} parse-expr) :field :values})
        {:values #{:SEMI} :error :EXPECT-SEMI}]
       (filter some?)
       (parse-pattern :STMT-PRINT tokens)
       ((fn [res] {:rest (:rest res)
                   :errors (:errors res)
                   :ast (merge {:values []} (:ast res))}))))

(defn ^:private parse-stmt-println [tokens]
  (->> [{:values #{:PRINTLN} :error :EXPECT-PRINTLN}
        (when-not (token/check (second tokens) #{:SEMI})
          {:function (gen-parse-repeated #{:COMMA} parse-expr) :field :values})
        {:values #{:SEMI} :error :EXPECT-SEMI}]
       (filter some?)
       (parse-pattern :STMT-PRINTLN tokens)
       ((fn [res] {:rest (:rest res)
                   :errors (:errors res)
                   :ast (merge {:values []} (:ast res))}))))

(defn ^:private parse-stmt-block [tokens]
  (if-not (token/check (first tokens) #{:LBRACE})
    {:ast {:type :STMT-BLOCK}
     :errors {:type :EXPECT-LBRACE :token (first tokens)}
     :rest tokens}
    (loop [nodes []
           errors []
           tokens0 (rest tokens)]
      (cond
        (token/check (first tokens0) #{:RBRACE})
        {:ast {:type :STMT-BLOCK :stmts nodes}
         :errors errors
         :rest (rest tokens0)}
        (token/check (first tokens0) #{:EOF})
        {:ast {:type :STMT-BLOCK}
         :errors [{:type :EXPECT-RBRACE :token (first tokens0)}]
         :rest tokens0}
        :else
        (let [res (parse-stmt tokens0)]
          (recur (conj nodes (:ast res))
                 (concat errors (:errors res))
                 (:rest res)))))))

(defn ^:private parse-stmt-if [tokens]
  (let [value1 (parse-pattern
                :STMT-IF tokens
                [{:values #{:IF} :error :EXPECT-IF}
                 {:function parse-expr :field :condition}
                 {:function parse-stmt-block :field :body}])
        tokens1 (:rest value1)]
    (if-not (token/check (first tokens1) #{:ELSE})
      value1
      (let [tokens2 (rest tokens1)
            value2 (if (token/check (first tokens2) #{:IF})
                     (parse-stmt-if tokens2)
                     (parse-stmt-block tokens2))]
        {:ast (merge (:ast value1) {:else (:ast value2)})
         :errors (concat (:errors value1) (:errors value2))
         :rest (:rest value2)}))))

(defn ^:private parse-stmt-while [tokens]
  (parse-pattern
   :STMT-WHILE tokens
   [{:values #{:WHILE} :error :EXPECT-WHILE}
    {:function parse-expr :field :condition}
    {:function parse-stmt-block :field :body}]))

(defn ^:private parse-stmt-each [tokens]
  (parse-pattern
   :STMT-EACH tokens
   [{:values #{:EACH} :error :EXPECT-EACH}
    {:values #{:ID} :error :EXPECT-ID :field :target}
    {:values #{:IN} :error :EXPECT-IN}
    {:values #{:LBRACKET} :error :EXPECT-LBRACKET}
    {:function parse-each-values :field :values}
    {:values #{:RBRACKET} :error :EXPECT-RBRACKET}
    {:function parse-stmt-block :field :body}]))

(defn ^:private parse-stmt [tokens]
  (case (->> tokens first :type)
    :PRINT (parse-stmt-print tokens)
    :PRINTLN (parse-stmt-println tokens)
    :LBRACE (parse-stmt-block tokens)
    :IF (parse-stmt-if tokens)
    :WHILE (parse-stmt-while tokens)
    :EACH (parse-stmt-each tokens)
    (parse-stmt-expression tokens)))

(defn ^:private parse-decl-variable [tokens]
  (parse-pattern
   :DECL-VARIABLE tokens
   [{:values #{:VAR} :error :EXPECT-VAR}
    {:values #{:ID} :error :EXPECT-ID :field :token}
    {:values #{:SEMI} :error :EXPECT-SEMI}]))

(defn ^:private parse-decl-program [tokens]
  (parse-pattern
   :DECL-PROGRAM tokens
   [{:values #{:PROGRAM} :error :EXPECT-PROGRAM}
    {:function parse-stmt-block :field :body}]))

(defn parse-decl [tokens]
  (case (->> tokens first :type)
    :VAR (parse-decl-variable tokens)
    :PROGRAM (parse-decl-program tokens)
    {:ast {}
     :errors [{:type :INVAL-DECL :token (first tokens)}]
     :rest (skip-until #{:VAR :PROGRAM} tokens)}))

(defn parse [tokens]
  (loop [nodes []
         errors []
         tokens0 tokens]
    (if (at-end? tokens0)
      {:ast nodes :errors errors :rest tokens0}
      (let [res (parse-decl tokens0)]
        (recur (conj nodes (:ast res))
               (concat errors (:errors res))
               (:rest res))))))

