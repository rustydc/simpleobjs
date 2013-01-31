Based heavily on Ian Piumarta's [ID object model](http://piumarta.com/software/id-objmodel/objmodel2.pdf)

### Ideas
 * Needs a language.
 * Expose as library?
 * Modularize globals?
 * Interfaces? Traits? Typeclasses?
 * Garbage collection?
 * Network support?
   * Via proxy object?
   * Serialization?
   * Garbage collection over network?
 * Expose HTTP for sends?
   * Interaction with JS maybe??
 * Async?
 * Data types?
   * Tuples
   * Array?
   * Ints, floats, bigints, doubles, rationals...
 * AST for this language?
   * EXPR = [ASSIGN] OBJECT [MSG ARGS ...]
   * Objects, variables? Scoping?
   * Then interpreter. (JIT?)
