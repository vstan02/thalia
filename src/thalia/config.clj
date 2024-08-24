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

(ns thalia.config
  (:gen-class)
  (:require [thalia.fs :as fs]
            [thalia.json :as json]))

(defn parse [path]
  (when-not path
    (throw (ex-info "Invalid config path." {:type :CONFIG-ERROR :path path})))
  (let [root (fs/parent path)
        cfg (json/parse (slurp path))
        nils (->> [:src :dest :target]
                  (filter #(nil? (% cfg))))]
    (when-not (empty? nils)
      (throw (ex-info "Required fields not found." {:type :CONFIG-ERROR :fields nils})))
    (let [dest (->> cfg :dest (fs/path root))]
      (merge
       {:cc "yasm -felf64"
        :stdlib (or (System/getenv "STDLIB_DIR")
                    "/usr/share/thalia/stdlib")}
       {:src (->> cfg :src (fs/path root))
        :dest dest
        :target (->> cfg :target (fs/path dest))}))))

