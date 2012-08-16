import nebula._

val imageClasses = Seq(
  "graffiti", 
  "trees", 
  "jpeg", 
  "boat", 
  "bark", 
  "bikes", 
  "light", 
  "wall").sorted

val otherImages = Seq(2, 4, 6)

val detectors = Seq(
  FASTDetector(maxKeyPoints = 100)
)

val extractors = for (
  patchWidth <- Seq(8, 4, 2, 1);
  color <- Seq(true, false)
) yield SortExtractor(
  normalizeRotation = false,
  normalizeScale = false,
  patchWidth = patchWidth,
  blurWidth = 5,
  color = color)

// val matchers = Seq(
//   L0Matcher(),
//   L1Matcher(),
//   KendallTauMatcher(),
//   CayleyMatcher())

val matchers = Seq(
  CayleyMatcher())

for (
  imageClass <- imageClasses;
  otherImage <- otherImages
) yield for (
  detector <- detectors;
  extractor <- extractors;
  matcher <- matchers
) yield CorrespondenceExperiment(
  imageClass, 
  otherImage,
  detector, 
  extractor, 
  matcher)

