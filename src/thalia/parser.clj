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
  (:gen-class))

(def ^:private parse-expr nil)
(def ^:private parse-stmt nil)

(defn ^:private check [token types]
  (contains? types (:type token)))

(defn ^:private parse-pattern [typ tokens rules]
  (loop [rls rules
         tkns tokens
         errors []
         ast {:type typ}]
    (if (or (empty? rls) (empty? tkns))
      {:rest tkns :errors errors :ast ast}
      (let [rule (first rls)
            token (first tkns)]
        (if-not (nil? (:values rule))
          (if (check token (:values rule))
            (recur (rest rls)
                   (rest tkns)
                   errors
                   (merge (when (:field rule) {(:field rule) token}) ast))
            {:rest (rest rls)
             :errors (conj errors {:type (:error rule) :token token})
             :ast ast})
          (let [res ((:function rule) tkns)]
            (recur (rest rls)
                   (:rest res)
                   (concat errors (:errors res))
                   (merge {(:field rule) (:ast res)} ast))))))))

(defn ^:private parse-each-values [tokens]
  (let [value1 (parse-expr tokens)
        tkns1 (:rest value1)]
    (if (check (first tkns1) #{:COLON})
      (let [value2 (->> tkns1 rest parse-expr)]
        {:rest (:rest value2)
         :errors (concat (:errors value1) (:errors value2))
         :ast {:from (:ast value1) :to (:ast value2)}})
      {:rest tkns1 :errors (:errors value1) :ast {:to (:ast value1)}})))

(defn ^:private parse-args [tokens]
  (let [res1 (parse-expr tokens)]
    (loop [tkns (:rest res1)
           errors (:errors res1)
           nodes [(:ast res1)]]
      (if-not (check (first tkns) #{:COMMA})
        {:rest tkns :errors errors :ast nodes}
        (let [res2 (->> tkns rest parse-expr)]
          (recur (:rest res2) (concat errors (:errors res2)) (concat nodes [(:ast res2)])))))))

(defn ^:private parse-expr-binary [tokens types parse-operand]
  (let [res1 (parse-operand tokens)]
    (loop [tkns (:rest res1)
           errors (:errors res1)
           node (:ast res1)]
      (if-not (check (first tkns) types)
        {:rest tkns :errors errors :ast node}
        (let [res2 (parse-pattern :EXPR-BINARY tkns
                                  [{:values (set types) :error :EXPECT-BINARY-OPERATOR :field :operator}
                                   {:function parse-operand :field :right}])]
          (recur (:rest res2) (concat errors (:errors res2)) (merge {:left node} (:ast res2))))))))

(defn ^:private parse-expr-primary [tokens]
  (let [token (first tokens)]
    (case (:type token)
      :LPAREN
      (parse-pattern :EXPR-GROUPING tokens
                     [{:values #{:LPAREN} :error :EXPECT-LPAREN}
                      {:function parse-expr :field :value}
                      {:values #{:RPAREN} :error :EXPECT-RPAREN}])
      :ID
      (parse-pattern :EXPR-VARIABLE tokens
                     [{:values #{:ID} :error :EXPECT-VARIABLE :field :token}])
      (parse-pattern :EXPR-LITERAL tokens
                     [{:values #{:INT} :error :EXPECT-LITERAL :field :token}]))))

(defn ^:private parse-expr-unary [tokens]
  (if-not (check (first tokens) #{:BANG :MINUS})
    (parse-expr-primary tokens)
    (parse-pattern :EXPR-UNARY tokens
                   [{:values #{:MINUS :BANG} :error :EXPECT-UNARY-OPERATOR :field :operator}
                    {:function parse-expr-unary :field :value}])))

(defn ^:private parse-expr-factor [tokens]
  (parse-expr-binary tokens
                     #{:STAR :SLASH :PERCENT}
                     parse-expr-unary))

(defn ^:private parse-expr-term [tokens]
  (parse-expr-binary tokens
                     #{:PLUS :MINUS}
                     parse-expr-factor))

(defn ^:private parse-expr-comp [tokens]
  (parse-expr-binary tokens
                     #{:GREATER :LESS :GREATER-EQUAL :LESS-EQUAL}
                     parse-expr-term))

(defn ^:private parse-expr-equal [tokens]
  (parse-expr-binary tokens
                     #{:EQUAL-EQUAL :BANG-EQUAL}
                     parse-expr-comp))

(defn ^:private parse-expr-logic-and [tokens]
  (parse-expr-binary tokens
                     #{:AMPER-AMPER}
                     parse-expr-equal))

(defn ^:private parse-expr-logic-or [tokens]
  (parse-expr-binary tokens
                     #{:PIPE-PIPE}
                     parse-expr-logic-and))

(defn ^:private parse-expr-assign [tokens]
  (let [res1 (parse-expr-logic-or tokens)]
    (if-not (check (->> res1 :rest first) #{:EQUAL})
      res1
      (let [res2 (->> res1 :rest rest parse-expr)]
        {:rest (:rest res2)
         :errors (concat (:errors res1) (:errors res2))
         :ast {:type :EXPR-ASSIGN
               :target (:ast res1)
               :value (:ast res2)}}))))

(defn ^:private parse-expr [tokens]
  (parse-expr-assign tokens))

(defn ^:private parse-stmt-expression [tokens]
  (parse-pattern :STMT-EXPRESSION tokens
                 [{:function parse-expr :field :value}
                  {:values #{:SEMI} :error :EXPECT-SEMI}]))

(defn ^:private parse-stmt-print [tokens]
  (->> [{:values #{:PRINT} :error :EXPECT-PRINT}
        (when-not (check (second tokens) #{:SEMI})
          {:function parse-args :field :values})
        {:values #{:SEMI} :error :EXPECT-SEMI}]
       (filter some?)
       (parse-pattern :STMT-PRINT tokens)
       ((fn [res] {:rest (:rest res)
                   :errors (:errors res)
                   :ast (merge {:values []} (:ast res))}))))

(defn ^:private parse-stmt-block [tokens]
  (if-not (check (first tokens) #{:LBRACE})
    {:rest tokens :errors {:type :EXPECT-LBRACE :token (first tokens)} :ast {:type :STMT-BLOCK}}
    (loop [tkns (rest tokens)
           errors []
           stmts []]
      (cond
        (check (first tkns) #{:RBRACE})
        {:rest (rest tkns) :errors errors :ast {:type :STMT-BLOCK :stmts stmts}}
        (check (first tkns) #{:EOF})
        {:rest tkns :errors [{:type :EXPECT-RBRACE :token (first tkns)}] :ast {:type :STMT-BLOCK}}
        :else
        (let [stmt (parse-stmt tkns)]
          (recur (:rest stmt) (concat errors (:errors stmt)) (concat stmts [(:ast stmt)])))))))

(defn ^:private parse-stmt-if [tokens]
  (parse-pattern :STMT-IF tokens
                 [{:values #{:IF} :error :EXPECT-IF}
                  {:function parse-expr :field :condition}
                  {:function parse-stmt-block :field :body}]))

(defn ^:private parse-stmt-while [tokens]
  (parse-pattern :STMT-WHILE tokens
                 [{:values #{:WHILE} :error :EXPECT-WHILE}
                  {:function parse-expr :field :condition}
                  {:function parse-stmt-block :field :body}]))

(defn ^:private parse-stmt-each [tokens]
  (parse-pattern :STMT-EACH tokens
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
    :LBRACE (parse-stmt-block tokens)
    :IF (parse-stmt-if tokens)
    :WHILE (parse-stmt-while tokens)
    :EACH (parse-stmt-each tokens)
    (parse-stmt-expression tokens)))

(defn ^:private parse-decl-variable [tokens]
  (parse-pattern :DECL-VARIABLE tokens
                 [{:values #{:VAR} :error :EXPECT-VAR}
                  {:values #{:ID} :error :EXPECT-ID :field :token}
                  {:values #{:SEMI} :error :EXPECT-SEMI}]))

(defn ^:private parse-decl-program [tokens]
  (parse-pattern :DECL-PROGRAM tokens
                 [{:values #{:PROGRAM} :error :EXPECT-PROGRAM}
                  {:function parse-stmt-block :field :body}]))

(defn ^:private parse-decl [tokens]
  (case (->> tokens first :type)
    :PROGRAM (parse-decl-program tokens)
    :VAR (parse-decl-variable tokens)
    {:rest (drop-while #(not (check % #{:VAR :PROGRAM})) tokens)
     :errors [{:type :INVAL-DECL :token (first tokens)}]
     :ast {}}))

(defn parse [tokens]
  (loop [tkns tokens
         errors []
         nodes []]
    (if (or (= (:type (first tkns)) :EOF) (empty? tkns))
      {:errors errors :ast nodes}
      (let [decl (parse-decl tkns)]
        (recur (:rest decl) (concat errors (:errors decl)) (conj nodes (:ast decl)))))))

