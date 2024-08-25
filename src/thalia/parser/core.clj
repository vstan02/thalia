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

(ns thalia.parser.core
  (:gen-class)
  (:require [thalia.token :as token]
            [thalia.parser.decl :as decl]))

(defn ^:private at-end? [tokens]
  (or (token/check (first tokens) #{:EOF}) (empty? tokens)))

(defn ^:private parse-file [tokens]
  (loop [nodes []
         errors []
         tokens0 tokens]
    (if (at-end? tokens0)
      {:ast nodes :errors errors :rest tokens0}
      (let [res (decl/parse tokens0)]
        (recur (conj nodes (:ast res))
               (concat errors (:errors res))
               (:rest res))))))

(defn parse [tokens]
  (let [file (parse-file tokens)]
    (if (->> file :errors empty?)
      (:ast file)
      (throw (ex-info "Invalid syntax."
                      (merge {:type :PARSER-ERROR} file))))))

