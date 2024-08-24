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

(ns thalia.core
  (:gen-class)
  (:require [clojure.string :as string]
            [thalia.io :as io]
            [thalia.fs :as fs]
            [thalia.json :as json]
            [thalia.config :as config]
            [thalia.lexer :as lexer]))

(defn ^:private src->asm [cfg src]
  (let [dest (->> src
                  (#(string/replace % (:src cfg) (:dest cfg)))
                  (#(string/replace % ".th" ".json")))]
    (->> (slurp src)
         (lexer/scan)
         (json/build)
         (spit dest))
    dest))

(defn -main [& args]
  (try
    (let [cfg (config/parse (first args))]
      (fs/clear-tree (:dest cfg))
      (->> (fs/get-all (:src cfg) ".*\\.th")
           (map #(src->asm cfg %))
           (string/join "\n")
           (io/writeln)))
    (catch Exception error
      (let [data (ex-data error)]
        (io/writeln "[ERROR]: " (ex-message error)
                    (when data (str "\n==> " (ex-data error))))))))

