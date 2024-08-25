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

(ns thalia.parser.stmt
  (:gen-class)
  (:require [thalia.token :as token]
            [thalia.parser.pattern :as pattern]
            [thalia.parser.expr :as expr]))

(def parse nil)

(defn ^:private at-end? [tokens]
  (or (token/check (first tokens) #{:EOF}) (empty? tokens)))

(defn ^:private parse-each-values [tokens]
  (let [value1 (expr/parse tokens)
        tokens1 (:rest value1)]
    (if (token/check (first tokens1) #{:COLON})
      (let [value2 (->> tokens1 rest expr/parse)
            tokens2 (:rest value2)]
        (if (token/check (first tokens2) #{:COLON})
          (let [value3 (->> tokens2 rest expr/parse)]
            {:ast {:from (:ast value1) :to (:ast value3) :step (:ast value2)}
             :errors (concat (:errors value1) (:errors value2) (:errors value3))
             :rest (:rest value3)})
          {:ast {:from (:ast value1) :to (:ast value2)}
           :errors (concat (:errors value1) (:errors value2))
           :rest (:rest value2)}))
      {:ast {:to (:ast value1)} :errors (:errors value1) :rest tokens1})))

(defn parse-block [tokens]
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
        (at-end? tokens0)
        {:ast {:type :STMT-BLOCK}
         :errors [{:type :EXPECT-RBRACE :token (first tokens0)}]
         :rest tokens0}
        :else
        (let [res (parse tokens0)]
          (recur (conj nodes (:ast res))
                 (concat errors (:errors res))
                 (:rest res)))))))

(defn parse-if [tokens]
  (let [parse-base-if (pattern/flat
                       :STMT-IF
                       [{:values #{:IF} :error :EXPECT-IF}
                        {:function expr/parse :field :condition}
                        {:function parse-block :field :body}])
        value1 (parse-base-if tokens)
        tokens1 (:rest value1)]
    (if-not (token/check (first tokens1) #{:ELSE})
      value1
      (let [tokens2 (rest tokens1)
            value2 (if (token/check (first tokens2) #{:IF})
                     (parse-if tokens2)
                     (parse-block tokens2))]
        {:ast (merge (:ast value1) {:else (:ast value2)})
         :errors (concat (:errors value1) (:errors value2))
         :rest (:rest value2)}))))

(def parse-while
  (pattern/flat
   :STMT-WHILE
   [{:values #{:WHILE} :error :EXPECT-WHILE}
    {:function expr/parse :field :condition}
    {:function parse-block :field :body}]))

(def parse-each
  (pattern/flat
   :STMT-EACH
   [{:values #{:EACH} :error :EXPECT-EACH}
    {:values #{:ID} :error :EXPECT-ID :field :target}
    {:values #{:IN} :error :EXPECT-IN}
    {:values #{:LBRACKET} :error :EXPECT-LBRACKET}
    {:function parse-each-values :field :values}
    {:values #{:RBRACKET} :error :EXPECT-RBRACKET}
    {:function parse-block :field :body}]))

(def parse-return
  (pattern/flat
   :STMT-RETURN
   [{:values #{:RETURN} :error :EXPECT-RETURN}
    {:function expr/parse :field :value}
    {:values #{:SEMI} :error :EXPECT-SEMI}]))

(def parse-expr
  (pattern/flat
   :STMT-EXPR
   [{:function expr/parse :field :value}
    {:values #{:SEMI} :error :EXPECT-SEMI}]))

(defn parse [tokens]
  (case (->> tokens first :type)
    :LBRACE (parse-block tokens)
    :RETURN (parse-return tokens)
    :IF (parse-if tokens)
    :WHILE (parse-while tokens)
    :EACH (parse-each tokens)
    (parse-expr tokens)))

