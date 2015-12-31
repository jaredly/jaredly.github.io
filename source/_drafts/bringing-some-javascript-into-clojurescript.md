title: Bringing some Javascript into Clojurescript
date: 2015-12-12 20:46:16
tags:
---
Or "if you're doing this, you probably haven't grokked The Clojurescript Way yet."

Using a local .edn file as you would a .json file

```
; ------- my-file.cljs -------
(def config (load-edn "./config.edn"))

; ------- macros.clj ---------
(ns om-tutorial.macros
  (:require [clojure.string :as str]))

(def filename-finder 10)

; magically get the filename of this file
(def dirname
  (str/join "/"
    (-> (meta #'filename-finder)
        :file
        (str/split #"/")
        drop-last)))

(defn rel-path [name]
  (str/join "/" [dirname name]))

(defmacro load-edn [filename]
  (read-string (slurp (rel-path filename))))
```
This requires that the macros file be in the same directory as the edn file you want :( which is not super ideal.