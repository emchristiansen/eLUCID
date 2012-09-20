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
  BRISKDetector(maxKeyPoints = 100)
)

// val extractors = for (
//   patchWidth <- Seq(8);
//   color <- Seq("Gray", "Lab")
// ) yield SortExtractor(
//   normalizeRotation = false,
//   normalizeScale = false,
//   patchWidth = patchWidth,
//   blurWidth = 5,
//   color = color)

val extractors = List(BRISKExtractor(true, true))

val matchers = Seq(
  L0Matcher())

for (
  detector <- detectors;
  extractor <- extractors;
  matcher <- matchers
) yield for (
  imageClass <- imageClasses;
  otherImage <- otherImages
) yield CorrespondenceExperiment(
  imageClass, 
  otherImage,
  detector, 
  extractor, 
  matcher)

