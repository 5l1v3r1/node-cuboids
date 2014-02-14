{RotationBasis} = require '../index'

testCreation = ->
  obj = new RotationBasis 3, 3, 3, 2, 1, 0
  console.log 'getDimensions() =>', obj.getDimensions()
  console.log 'getPowers() =>', obj.getPowers()

testStandard = ->
  standard = RotationBasis.standard 3, 3, 4
  console.log 'getDimensions() =>', standard.getDimensions()
  console.log 'getPowers() =>', standard.getPowers()

testIsSubset = ->
  container = new RotationBasis 3, 4, 3, 2, 1, 2
  subfield = new RotationBasis 3, 4, 3, 2, 2, 0
  if not container.isSubset subfield
    console.log 'isSubset did not work'
  else console.log 'isSubset worked'

testCreation()
testStandard()
testIsSubset()
