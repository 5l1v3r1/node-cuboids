# Cuboids

Cuboids is a C API I wrote in my spare time to manipulate and solve cuboid puzzles (for example, the Rubik's cube). Cuboids itself provides a mechanism for cuboid input from the console, algorithm parsing, move application, heuristic indexing, solution searching, and the works.

This binding is not made to provide the full functionality of cuboids, but rather the parts which are appropriate for a JavaScript application. For example, this binding allows the application of cuboids, conversion from piece data to sticker data, and, soon, algorithm parsing.

# Usage

The heart of `node-cuboid` is the Cuboid object. This object represents a cuboid, and is created by passing the dimensions to its constructor:

    var Cuboid = require('node-cuboids').Cuboid;
    var instance = new Cuboid(3, 3, 3);

You can apply an instance of a cuboid to another cuboid of equal dimensions:

    var product = left.multiply(right);

You can access piece information using `getEdge()`, `getCorner()`, and `getCenter()`, all of which take an index.
