{StickerMap, Cuboid} = require '../index'

logFunc = console.log

testGeneral = (s) ->
  logFunc 'countStickers() =>', s.countStickers()
  logFunc 'countStickersForFace(2) =>', s.countStickersForFace 2
  logFunc 'faceStartIndex(3) =>', s.faceStartIndex 3
  
  logFunc 'dimensionsOfFace(3) =>', s.dimensionsOfFace 3
  logFunc 'indexToPoint(3, 2) =>', s.indexToPoint 3, 2
  logFunc 'indexFromPoint(3, 1, 1) =>', s.indexFromPoint 3, 1, 1
  
  logFunc 'getSticker(10) =>', s.getSticker 10
  logFunc 'setSticker(10, 1) =>', s.setSticker 10, 1
  logFunc 'getSticker(10) =>', s.getSticker 10

testConversion = ->
  c = new Cuboid 3, 3, 3
  s = new StickerMap c
  c1 = s.getCuboid()
  s1 = new StickerMap s # copy operation
  logFunc 's.getDimensions() =>', s.getDimensions()
  logFunc 'c1.getDimensions() =>', c1.getDimensions()
  logFunc 's1.getDimensions() =>', s1.getDimensions()

testException = ->
  logFunc 'testing conversion exception...'
  s = new StickerMap 3, 3, 3
  try
    c = s.getCuboid()
    logFunc 'should have thrown exception'
  catch e
    logFunc 'passed:', e

example = new StickerMap new Cuboid 3, 3, 3
testGeneral example
testConversion()
testException()

# test memory management
###
logFunc = ->
loop
  example = new StickerMap new Cuboid 3, 3, 3
  testGeneral example
  testConversion()
  testException()
###