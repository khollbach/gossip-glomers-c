# Kevan's notes from skimming json-c api docs

* The section "Using json-c" in the main page is pretty good (and short!)
  https://json-c.github.io/json-c/json-c-0.16/doc/html/index.html#using
  * seems to confirm that there's no "better way" to build JSON objects than
    the verbose thing we're doing
  * mentions `put` recursing on child nodes, so that it frees all the nodes in
    the tree -- just call it once on the root
  * answers a question we had about `json_object_to_json_string`: who frees the
    returned string? Answer: it gets freed automatically when the `json_object`
    drops to zero references. (So use it only before then, and don't free it
    manually)

* docs for `get` and `put` don't go into much more detail than we've already
  seen
    * `get` does give an example use-case for calling it explicitly, which is
      nice: using an object field (retrieved thru `json_object_object_get`)
      beyond the lifetime of the parent object
