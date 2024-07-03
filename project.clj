(defproject thalia ""
  :description "A fast, statically typed, general-purpose, procedural programming language"
  :url "https://github.com/vstan02/thalia"
  :license {:name "GPL-3.0"}
  :dependencies [[org.clojure/clojure "1.11.1"]
                 [cheshire "5.13.0"]]
  :main ^:skip-aot thalia.core
  :target-path "target/%s"
  :profiles {:uberjar {:aot :all
                       :jvm-opts ["-Dclojure.compiler.direct-linking=true"]}})
