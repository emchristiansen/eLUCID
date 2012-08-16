import nebula._

val detector = FASTDetector(maxKeyPoints = 102)

val extractor = SortExtractor(
  normalizeRotation = false,
  normalizeScale = false,
  patchWidth = 8,
  blurWidth = 5,
  color = false)

val matcher = L0Matcher()

val experiment = CorrespondenceExperiment(
  imageClass = "bikes",
  otherImage = 4,
  detector = detector,
  extractor = extractor,
  matcher = matcher)

Seq(experiment)
