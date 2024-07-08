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

(ns thalia.lexer
  (:gen-class)
  (:require [clojure.string :as string]
            [thalia.token :as token]))

(def ^:private base-tokens
  {"(" :LPAREN
   ")" :RPAREN
   "{" :LBRACE
   "}" :RBRACE
   "[" :LBRACKET
   "]" :RBRACKET
   "," :COMMA
   ";" :SEMI
   ":" :COLON
   "." :DOT
   "+" :PLUS
   "-" :MINUS
   "*" :STAR
   "\\" :SLASH
   "%" :PERCENT
   "!" :BANG
   "=" :EQUAL
   "==" :EQUAL-EQUAL
   "!=" :BANG-EQUAL
   ">" :GREATER
   ">=" :GREATER-EQUAL
   "<" :LESS
   "<=" :LESS-EQUAL
   "&&" :AMPER-AMPER
   "||" :PIPE-PIPE})

(def ^:private kws-tokens
  {"def" :VAR
   "print" :PRINT
   "program" :PROGRAM
   "if" :IF
   "else" :ELSE
   "elif" :ELIF
   "while" :WHILE
   "each" :EACH
   "in" :IN
   "return" :RETURN
   "static" :STATIC
   "extern" :EXTERN})

(defn ^:private whitespace? [c]
  (contains? #{\tab \space \return \newline} c))

(defn ^:private digit? [c]
  (Character/isDigit c))

(defn ^:private alpha? [c]
  (or (= c \_) (Character/isLetter c)))

(defn ^:private alphanum? [c]
  (or (digit? c) (alpha? c)))

(defn ^:private cond-subs [pred s]
  (->> (take-while pred s)
       (string/join "")))

(defn ^:private scan-number-token [code pos line]
  (token/make :INT (cond-subs digit? code) pos line))

(defn ^:private scan-id|kw-token [code pos line]
  (let [value (cond-subs alphanum? code)
        ttype (kws-tokens value)]
    (token/make (if ttype ttype :ID) value pos line)))

(defn ^:private scan-base-token [code pos line]
  (let [value1 (str (first code))
        value2 (str (first code) (second code))
        ttype2 (base-tokens value2)]
    (if (some? ttype2)
      (token/make ttype2 value2 pos line)
      (token/make (base-tokens value1) value1 pos line))))

(defn ^:private scan-next [code pos line]
  (let [ws (take-while whitespace? code)
        ws-size (count ws)
        trimmed (drop ws-size code)
        fst (first trimmed)
        nxt-pos (+ ws-size pos)
        nxt-line (+ (count (filter #{\newline} ws)) line)]
    (cond
      (nil? fst) (token/make :EOF "" nxt-pos nxt-line)
      (digit? fst) (scan-number-token trimmed nxt-pos nxt-line)
      (alpha? fst) (scan-id|kw-token trimmed nxt-pos nxt-line)
      :else (scan-base-token trimmed nxt-pos nxt-line))))

(defn ^:private scan-all [src]
  (loop [code src
         pos 0
         line 1
         tkns []]
    (let [tkn (scan-next code pos line)
          nxt-pos (+ (:pos tkn) (count (:value tkn)))]
      (if (token/check tkn #{:EOF})
        (conj tkns tkn)
        (recur (drop (- nxt-pos pos) code)
               nxt-pos
               (:line tkn)
               (conj tkns tkn))))))

(defn scan [code]
  (let [tokens (scan-all code)
        errors (->> tokens (filter #(nil? (:type %))))]
    {:errors errors :tokens tokens}))

