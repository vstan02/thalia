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

(ns thalia.parser.decl
  (:gen-class)
  (:require [thalia.token :as token]
            [thalia.parser.pattern :as pattern]
            [thalia.parser.stmt :as stmt]))

(defn ^:private skip-until [types tokens]
  (drop-while #(not (token/check % types)) tokens))

(def parse-param
  (pattern/flat
   :DECL-PARAM
   [{:values #{:ID} :error :EXPECT-ID :field :name}
    {:values #{:COLON} :error :EXPECT-COLON}
    {:values #{:ID} :error :EXPECT-ID :field :data-type}]))

(defn ^:private parse-params [tokens]
  (if (token/check (first tokens) #{:EOF :RPAREN})
    {:ast [] :errors [] :rest tokens}
    (let [res1 (parse-param tokens)]
      (loop [nodes1 [(:ast res1)]
             errors1 (:errors res1)
             tokens1 (:rest res1)]
        (if-not (token/check (first tokens1) #{:COMMA})
          {:ast nodes1 :errors errors1 :rest tokens1}
          (let [res2 (->> tokens1 rest parse-param)]
            (recur (conj nodes1 (:ast res2))
                   (concat errors1 (:errors res2))
                   (:rest res2))))))))

(def parse-use
  (pattern/flat
   :DECL-USE
   [{:values #{:USE} :error :EXPECT-USE}
    {:values #{:ID} :error :EXPECT-ID :field :name}
    {:values #{:LPAREN} :error :EXPECT-LPAREN}
    {:function parse-params :field :params}
    {:values #{:RPAREN} :error :EXPECT-RPAREN}
    {:values #{:COLON} :error :EXPECT-COLON}
    {:values #{:ID} :error :EXPECT-ID :field :data-type}
    {:values #{:SEMI} :error :EXPECT-SEMI}]))

(def parse-fun
  (pattern/flat
   :DECL-FUN
   [{:values #{:GLOBAL :LOCAL} :error :EXPECT-FUN-CLASS :field :class}
    {:values #{:ID} :error :EXPECT-ID :field :name}
    {:values #{:LPAREN} :error :EXPECT-LPAREN}
    {:function parse-params :field :params}
    {:values #{:RPAREN} :error :EXPECT-RPAREN}
    {:values #{:COLON} :error :EXPECT-COLON}
    {:values #{:ID} :error :EXPECT-ID :field :data-type}
    {:function stmt/parse-block :field :body}]))

(defn parse [tokens]
  (case (->> tokens first :type)
    :USE (parse-use tokens)
    :GLOBAL (parse-fun tokens)
    :LOCAL (parse-fun tokens)
    {:ast {}
     :errors [{:type :INVAL-DECL :token (first tokens)}]
     :rest (skip-until #{:USE :GLOBAL :LOCAL} tokens)}))

