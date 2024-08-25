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

(ns thalia.parser.expr
  (:gen-class)
  (:require [thalia.token :as token]
            [thalia.parser.pattern :as pattern]))

(def parse nil)

(defn ^:private base-binary [types parse-operand]
  (fn [tokens]
    (let [parse-right (pattern/flat
                       :EXPR-BINARY
                       [{:values types :error :EXPECT-BINARY-OPERATOR :field :operator}
                        {:function parse-operand :field :right}])
          res1 (parse-operand tokens)]
      (if-not (token/check (->> res1 :rest first) types)
        res1
        (let [res2 (parse-right (:rest res1))]
          {:ast (merge {:left (:ast res1)} (:ast res2))
           :errors (concat (:errors res1) (:errors res2))
           :rest (:rest res2)})))))

(defn ^:private repeated-binary [types parse-operand]
  (fn [tokens]
    (let [parse-right (pattern/flat
                       :EXPR-BINARY
                       [{:values types :error :EXPECT-BINARY-OPERATOR :field :operator}
                        {:function parse-operand :field :right}])
          res1 (parse-operand tokens)]
      (loop [node (:ast res1)
             errors (:errors res1)
             tokens0 (:rest res1)]
        (if-not (token/check (first tokens0) types)
          {:ast node :errors errors :rest tokens0}
          (let [res2 (parse-right tokens0)]
            (recur (merge {:left node} (:ast res2))
                   (concat errors (:errors res2))
                   (:rest res2))))))))

(def parse-literal
  (pattern/flat
   :EXPR-LITERAL
   [{:values #{:INT} :error :EXPECT-LITERAL :field :token}]))

(def parse-id
  (pattern/flat
   :EXPR-ID
   [{:values #{:ID} :error :EXPECT-ID :field :token}]))

(def parse-paren
  (pattern/flat
   :EXPR-PAREN
   [{:values #{:LPAREN} :error :EXPECT-LPAREN}
    {:function parse :field :value}
    {:values #{:RPAREN} :error :EXPECT-RPAREN}]))

(defn parse-primary [tokens]
  (case (->> tokens first :type)
    :LPAREN (parse-paren tokens)
    :ID (parse-id tokens)
    (parse-literal tokens)))

(defn parse-unary [tokens]
  ((if-not (token/check (first tokens) #{:BANG :MINUS})
     parse-primary
     (pattern/flat
      :EXPR-UNARY
      [{:values #{:MINUS :BANG} :error :EXPECT-UNARY-OPERATOR :field :operator}
       {:function parse-unary :field :value}]))
   tokens))

(def parse-factor
  (repeated-binary #{:STAR :SLASH :PERCENT} parse-unary))

(def parse-term
  (repeated-binary #{:PLUS :MINUS} parse-factor))

(def parse-comp
  (base-binary #{:GREATER :LESS :GREATER-EQUAL :LESS-EQUAL} parse-term))

(def parse-equal
  (base-binary #{:EQUAL-EQUAL :BANG-EQUAL} parse-comp))

(def parse-logic-and
  (repeated-binary #{:AMPER-AMPER} parse-equal))

(def parse-logic-or
  (repeated-binary #{:PIPE-PIPE} parse-logic-and))

(defn parse-assign [tokens]
  (let [res1 (parse-logic-or tokens)]
    (if-not (token/check (->> res1 :rest first) #{:EQUAL})
      res1
      (let [res2 (->> res1 :rest rest parse)]
        {:ast {:type :EXPR-ASSIGN
               :target (:ast res1)
               :value (:ast res2)}
         :errors (concat (:errors res1) (:errors res2))
         :rest (:rest res2)}))))

(def parse parse-assign)

