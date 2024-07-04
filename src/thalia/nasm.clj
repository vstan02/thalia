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

(ns thalia.nasm
  (:gen-class)
  (:require [clojure.string :as string]))

(def NASM-DATA
  (str "section .data\n"
       "\tfalse equ 0\n"
       "\ttrue equ 1\n"
       "\tnull equ 0\n"
       "\t_SYS_EXIT_ equ 0x3c\n"
       "\t_SYS_WRITE_ equ 0x1\n"
       "\t_SYS_STDOUT_ equ 1\n"))

(def NASM-BSS
  (str "\nsection .bss\n"
       "\t_CHR_BSS_ resb 1\n"))

(def NASM-END
  (str "int_print:\n"
       "\tpush rax\n"
       "\tpush rbx\n"
       "\tpush rcx\n"
       "\tpush rdx\n"
       "\txor rcx, rcx\n"
       "\tcmp rax, 0\n"
       "\tjge .decomp\n"
       ".negate:\n"
       "\tmov rdx, rax\n"
       "\tmov rax, '-'\n"
       "\tcall chr_print\n"
       "\tmov rax, rdx\n"
       "\tneg rax\n"
       ".decomp:\n"
       "\tmov rbx, 10\n"
       "\txor rdx, rdx\n"
       "\tdiv rbx\n"
       "\tadd rdx, '0'\n"
       "\tpush rdx\n"
       "\tinc rcx\n"
       "\tcmp rax, 0\n"
       "\tje .print\n"
       "\tjmp .decomp\n"
       ".print:\n"
       "\tcmp rcx, 0\n"
       "\tje .close\n"
       "\tdec rcx\n"
       "\tpop rax\n"
       "\tcall chr_print\n"
       "\tjmp .print\n"
       ".close:\n"
       "\tpop rdx\n"
       "\tpop rcx\n"
       "\tpop rbx\n"
       "\tpop rax\n"
       "\tret\n\n"
       "eol_print:\n"
       "\tpush rax\n"
       "\tmov rax, 0xA\n"
       "\tcall chr_print\n"
       "\tpop rax\n"
       "\tret\n\n"
       "chr_print:\n"
       "\tpush rax\n"
       "\tpush rdi\n"
       "\tpush rsi\n"
       "\tpush rdx\n"
       "\tpush rcx\n"
       "\tmov [_CHR_BSS_], al\n"
       "\tmov rax, _SYS_WRITE_\n"
       "\tmov rdi, _SYS_STDOUT_\n"
       "\tmov rsi, _CHR_BSS_\n"
       "\tmov rdx, 1\n"
       "\tsyscall\n"
       "\tpop rcx\n"
       "\tpop rdx\n"
       "\tpop rsi\n"
       "\tpop rdi\n"
       "\tpop rax\n"
       "\tret\n\n"
       "sys_exit:\n"
       "\tmov rax, _SYS_EXIT_\n"
       "\txor rdi, rdi\n"
       "\tsyscall\n"))

(def make)

(defn ^:private next! [label]
  (swap! label inc))

(defn ^:private make-expr-literal [node _]
  (str "\tpush " (get-in node [:token :value]) "\n"))

(defn ^:private make-expr-variable [node _]
  (str "\tpush qword [" (get-in node [:token :value]) "]\n"))

(defn ^:private make-expr-grouping [node label]
  (make (:value node) label))

(defn ^:private make-expr-unary [node label]
  (str (make (:value node) label)
       "\tpop rax\n"
       (case (get-in node [:operation :type])
         :MINUS "\tneg rax\n"
         :BANG "\tnot rax\n")
       "\tpush rax\n"))

(defn ^:private make-expr-binary [node label]
  (str (make (:left node) label)
       (make (:right node) label)
       "\tpop rbx\n\tpop rax\n"
       (case (get-in node [:operation :type])
         :PLUS "\tadd rax, rbx\n"
         :MINUS "\tsub rax, rbx\n"
         :STAR "\timul rax, rbx\n"
         :SLASH "\tidiv rbx\n"
         :PERCENT "\txor rdx, rdx\n\tidiv rbx\n\tmov rax, rdx\n"
         :EQUAL-EQUAL (str "\tmov rdx, 0\n\tcmp rax, rbx\n\tjne .ll" (next! label) "\n"
                           "\tmov rdx, 1\n.ll" @label ":\n\tmov rax, rdx\n")
         :BANG-EQUAL (str "\tmov rdx, 1\n\tcmp rax, rbx\n\tjne .ll" (next! label) "\n"
                          "\tmov rdx, 0\n.ll" @label ":\n\tmov rax, rdx\n")
         :GREATER-EQUAL (str "\tmov rdx, 0\n\tcmp rax, rbx\n\tjl .ll" (next! label) "\n"
                             "\tmov rdx, 1\n.ll" @label ":\n\tmov rax, rdx\n")
         :LESS (str "\tmov rdx, 1\n\tcmp rax, rbx\n\tjl .ll" (next! label) "\n"
                    "\tmov rdx, 0\n.ll" @label ":\n\tmov rax, rdx\n")
         :LESS-EQUAL (str "\tmov rdx, 0\n\tcmp rax, rbx\n\tjg .ll" (next! label) "\n"
                          "\tmov rdx, 1\n.ll" @label ":\n\tmov rax, rdx\n")
         :GREATER (str "\tmov rdx, 1\n\tcmp rax, rbx\n\tjg .ll" (next! label) "\n"
                       "\tmov rdx, 0\n.ll" @label ":\n\tmov rax, rdx\n"))
       "\tpush rax\n"))

(defn ^:private make-expr-assign [node label]
  (str (make (:value node) label)
       "\tpop qword [" (get-in node [:target :name :value]) "]\n"
       (make (:target node) label)))

(defn ^:private make-stmt-expression [node label]
  (str (make (:value node) label)
       "\tpop rax\n"))

(defn ^:private make-stmt-print [node label]
  (->> (:values node)
       (map #(str (make % label)
                  "\tpop rax\n\tcall int_print\n\tmov rax, ' '\n\tcall chr_print\n"))
       (string/join "")
       (#(str % "\tcall eol_print\n"))))

(defn ^:private make-stmt-block [node label]
  (->> (:stmts node)
       (map #(make % label))
       (string/join "")))

(defn ^:private make-stmt-program [node label]
  (str "\nsection .text\n_start:\n"
       (make (:body node) label)
       "\tcall sys_exit\n\n"))

(def ^:private make-funcs
  {:EXPR-LITERAL make-expr-literal
   :EXPR-VARIABLE make-expr-variable
   :EXPR-GROUPING make-expr-grouping
   :EXPR-UNARY make-expr-unary
   :EXPR-BINARY make-expr-binary
   :EXPR-ASSIGN make-expr-assign
   :STMT-EXPRESSION make-stmt-expression
   :STMT-PRINT make-stmt-print
   :STMT-BLOCK make-stmt-block
   :STMT-PROGRAM make-stmt-program})

(defn ^:private make [node label]
  ((make-funcs (:type node)) node label))

(defn translate [nodes]
  (let [label (atom 0)
        program (->> nodes
                     (filter #(= (:type %) :STMT-PROGRAM))
                     (first))]
    (->> nodes
         (filter #(not= (:type %) :STMT-PROGRAM))
         (map #(make % label))
         (string/join "")
         (#(str "global _start\n\n"
                NASM-DATA
                %
                NASM-BSS
                (make program label)
                NASM-END)))))

