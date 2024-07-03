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

(defn ^:private match [typ tokens rules]
  (loop [rls rules
         tkns tokens
         errors []
         ast {:type typ}]
    (if (or (empty? rls) (= (:type (first tkns)) :EOF))
      {:rest tkns :errors errors :ast ast}
      (let [rule (first rls)
            token (first tkns)]
        (if-not (nil? (:values rule))
          (if (contains? (:values rule) (:type token))
            (recur (rest rls) (rest tkns) errors (merge (when (:field rule) {(:field rule) token}) ast))
            {:rest (rest tkns) :errors (conj errors {:type (:error rule) :token token}) :ast ast})
          (let [res ((:function rule) tkns)]
            (recur (rest rls) (:rest res) (concat errors (:errors res)) (merge {(:field rule) (:ast res)} ast))))))))

(defn ^:private parse-decl-param [tokens]
  (match :DECL_PARAM tokens
         [{:values #{:ID} :error :INV_PARAM_TYPE :field :data-type}
          {:values #{:ID} :error :INV_PARAM_NAME :field :name}]))

(defn ^:private parse-decl-params [tokens]
  (if (contains? #{:RPAREN :EOF} (:type (first tokens)))
    {:rest tokens :errors [] :ast []}
    (let [param (parse-decl-param tokens)]
      (loop [tkns (:rest param)
             errors (:errors param)
             params [(:ast param)]]
        (if (or (not= (:type (first tkns)) :COMMA) (seq errors))
          {:rest tkns :errors errors :ast params}
          (let [rst (rest tkns)
                nxt (parse-decl-param rst)]
            (recur rst (concat errors (:errors nxt)) (conj params (:ast nxt)))))))))

(defn ^:private parse-decl-func [tokens]
  (match :DECL_FUNC tokens
         [{:values #{:EXTERN :STATIC} :error :INV_FUNC_CLASS :field :class}
          {:values #{:ID} :error :INV_RET_TYPE :field :ret-type}
          {:values #{:ID} :error :INV_FUNC_NAME :field :name}
          {:values #{:LPAREN} :error :LPAREN_EXPECT}
          {:function parse-decl-params :error :INV_FUNC_PARAMS :field :params}
          {:values #{:RPAREN} :error :RPAREN_EXPECT}
          {:values #{:SEMI} :error :SEMI_EXPECT}]))

(defn ^:private parse-decl [tokens]
  (parse-decl-func tokens))

(defn parse [tokens]
  (loop [tkns tokens
         errors []
         ast []]
    (if (= (:type (first tkns)) :EOF)
      {:errors errors :ast ast}
      (let [decl (parse-decl tkns)]
        (recur (:rest decl) (concat errors (:errors decl)) (conj ast (:ast decl)))))))

