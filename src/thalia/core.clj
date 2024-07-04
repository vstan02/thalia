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
  (:require [thalia.nasm :as nasm]))

(defn -main []
  (println
   (nasm/translate
    [{:type :STMT-PROGRAM
      :body {:type :STMT-BLOCK
             :stmts [{:type :STMT-PRINT
                      :values [{:type :EXPR-BINARY
                                :operation {:type :STAR}
                                :left {:type :EXPR-UNARY
                                       :operation {:type :MINUS}
                                       :value {:type :EXPR-LITERAL
                                               :token {:type :INT
                                                       :value "124"}}}
                                :right {:type :EXPR-GROUPING
                                        :value {:type :EXPR-BINARY
                                                :operation {:type :PLUS}
                                                :left {:type :EXPR-LITERAL
                                                       :token {:type :INT
                                                               :value "35"}}
                                                :right {:type :EXPR-LITERAL
                                                        :token {:type :INT
                                                                :value "106"}}}}}]}]}}])))

