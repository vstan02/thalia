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

(ns thalia.parser.pattern
  (:gen-class)
  (:require [thalia.token :as token]))

(defn flat [nname rules]
  (fn [tokens]
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
                     (:rest res)))))))))

