import nebula._
import nebula.SortDescriptor._

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

val extractors = for (
  patchWidth <- Seq(8, 16, 24)
) yield Seq(
  NCCExtractor(
    false,
    false,
    25,
    5,
    "Gray"),
  NormalizeExtractor(
    false,
    false,
    25,
    5,
    "Gray"),
  RankExtractor(
    false,
    false,
    25,
    5,
    "Gray"))

val matchers = Seq(
  L1Matcher(),
  L2Matcher()
)

// val matchers = Seq(
//   L0Matcher()
// )

for (
  detector <- detectors;
  extractor <- extractors.flatten;
  matcher <- matchers
) yield for (
  imageClass <- imageClasses;
  otherImage <- otherImages
) yield implicitly[WideBaselineExperiment => WideBaselineExperiment].apply(
  WideBaselineExperiment(
    imageClass, 
    otherImage,
    detector, 
    extractor, 
    matcher))

