import nebula._

val imageClasses = Seq(
  "bikes", 
  "light").sorted

// val imageClasses = Seq(
//   "graffiti", 
//   "trees", 
//   "jpeg", 
//   "boat", 
//   "bark", 
//   "bikes", 
//   "light", 
//   "wall").sorted

val otherImages = Seq(4)

val detectors = Seq(
  FASTDetector(maxKeyPoints = 2)
//  BRISKDetector(maxKeyPoints = 100)
)

// val extractors = Seq(
//   RawExtractor(false, false, 8, 5, "Gray")
// )

// val matchers = Seq(
//   CayleyMatcher(),
//   RobustCayleyMatcher()
// )

val extractorsAndMatchers = Seq(
  // (SortExtractor(
  //   false,
  //   false,
  //   8,
  //   5,
  //   "Gray"),
  //  CayleyMatcher()),
  // (SortExtractor(
  //   false,
  //   false,
  //   8,
  //   5,
  //   "Gray"),
  //  L0Matcher()),
  (RawExtractor(
    false,
    false,
    4,
    5,
    "Gray"),
   GeneralizedL0Matcher())
  // (RawExtractor(
  //   false,
  //   false,
  //   8,
  //   5,
  //   "Gray"),
  //  RobustCayleyMatcher)
)

for (
  detector <- detectors;
  (extractor, matcher) <- extractorsAndMatchers
) yield for (
  imageClass <- imageClasses;
  otherImage <- otherImages
) yield CorrespondenceExperiment(
  imageClass, 
  otherImage,
  detector, 
  extractor, 
  matcher)

