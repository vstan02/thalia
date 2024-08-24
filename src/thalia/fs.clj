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

(ns thalia.fs
  (:gen-class)
  (:require [babashka.fs :as fs]))

(defn get-all [root regex]
  (->> (fs/match root (str "regex:" regex) {:recursive true})
       (map str)))

(defn clear-tree [path]
  (fs/delete-tree path)
  (fs/create-dirs path))

(defn by [root file]
  (->> file (fs/path root) fs/canonicalize str))

(defn parent [file]
  (->> file fs/parent fs/canonicalize str))

