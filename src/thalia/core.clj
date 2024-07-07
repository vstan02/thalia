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
  (:require [thalia.nasm :as nasm]
            [thalia.lexer :as lexer]
            [thalia.parser :as parser]))

(defn -main [& args]
  (->> (first args)
       (slurp)
       (lexer/scan)
       (:tokens)
       (parser/parse)
       (:ast)
       (nasm/translate)
       (println)))

