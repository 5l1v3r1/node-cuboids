{Cuboid} = require '../index'

c = new Cuboid 3, 3, 3

testInformational = ->
  # console.log 'testing informational methods...'
  console.log 'getDimensions() =>', c.getDimensions()
  console.log 'countEdgesForDedge(0) =>', c.countEdgesForDedge 0
  console.log 'countCentersForFace(1) =>', c.countCentersForFace 1
  console.log 'countEdges() =>', c.countEdges()
  console.log 'countCenters() =>', c.countCenters()
  console.log 'edgeIndex(5, 0) =>', c.edgeIndex 5, 0
  console.log 'centerIndex(5, 0) =>', c.centerIndex 5, 0
  console.log 'getEdge(0) =>', c.getEdge 0
  console.log 'getCenter(0) =>', c.getCenter 0
  console.log 'getCorner(0) =>', c.getCorner 0
  console.log 'done.'

testInformationalMemory = ->
  c.getDimensions()
  c.countEdgesForDedge 0
  c.countCentersForFace 1
  c.countEdges()
  c.countCenters()
  c.edgeIndex 5, 0
  c.centerIndex 5, 0
  c.getEdge 0
  c.getCenter 0
  c.getCorner 0

testMultiplication = ->
  console.log 'performing multiplication...'
  c2 = new Cuboid 3, 3, 3
  c = c.multiply c2
  console.log 'done.'

testMultiplicationMemory = ->
  c2 = new Cuboid 3, 3, 3
  c = c.multiply c2

testSetting = ->
  # meh, who cares if it creates an invalid cube!
  c.setEdge 2, symmetry: 1, dedgeIndex: 2, edgeIndex: 0
  console.log 'getEdge(2) =>', c.getEdge 2
  c.setCorner 2, symmetry: 4, index: 2
  console.log 'getCorner(2) =>', c.getCorner 2
  c.setCenter 2, side: 2, index: 0
  console.log 'getCenter(2) =>', c.getCenter 2

testInformational()
testMultiplication()
testInformational()
testSetting()

# use to test for leaks
# loop
  # testInformationalMemory()
  # testMultiplicationMemory()
