// import java.io.File

// case class ROCData(val predictions: List[Double], val truths: List[Boolean]) { 
//   assert(predictions.sorted == predictions)
//   assert(predictions.size == truths.size)

//   def prettyPrint: String = {
//     val (fprs, tprs) = curve.unzip

//     List(Util.truncate(predictions), truths.mkString(" "), Util.truncate(fprs), Util.truncate(tprs)).mkString("\n")
//   }

//   val curve: List[Tuple2[Double, Double]] = {
//     val truthInts = truths.map(b => if (b) 1 else 0)
    
//     val truePositiveScan = truthInts.scanLeft(0)(_ + _)
//     val falsePositiveScan = truePositiveScan.zipWithIndex.map({case (tp, n) => n - tp})

//     val pairs = for ((truePositives, falsePositives) <- truePositiveScan.zip(falsePositiveScan)) yield {
//       val numPositive = truthInts.sum
//       val numNegative = truthInts.size - numPositive      

//       val truePositiveRate = truePositives.toDouble / numPositive.toDouble
//       val falsePositiveRate = falsePositives.toDouble / numNegative.toDouble
//       (falsePositiveRate, truePositiveRate)
//     }

//     // def heads[A](list: List[A]): Stream[List[A]] = list.inits.toStream.reverse

//     // val pairs = for (guessPositive <- heads(truthInts)) yield {
//     //   val truePositives = guessPositive.sum
//     //   val falsePositives = guessPositive.size - truePositives

//     //   val numPositive = truthInts.sum
//     //   val numNegative = truthInts.size - numPositive      

//     //   val truePositiveRate = truePositives.toDouble / numPositive.toDouble
//     //   val falsePositiveRate = falsePositives.toDouble / numNegative.toDouble
//     //   (falsePositiveRate, truePositiveRate)
//     // }
//     assert(pairs.sortBy(_._1) == pairs)

//     // For each fpr, we will record only the highest tpr.
//     val reducedPairs = {
//       // The tprs are in ascending order, so the map will retain only the highest tprs.
//       val map = pairs.toMap
//       for (k <- map.keys.toList.sorted) yield (k, map(k))
//     }

//     reducedPairs
//   }

//   def curveString: String = {
//     val (fprs, tprs) = curve.unzip
//     List(fprs.mkString(" "), tprs.mkString(" ")).mkString("\n")
//   }

//   def tprFromFPR(fpr: Double): Double = {
//     val map = curve.toMap
//     if (map.contains(fpr)) { 
//       map(fpr)
//     } else { 
//       val (smaller, bigger) = curve.partition(_._1 < fpr)
//       val (smallerFPR, smallerTPR) = smaller.last
//       val (biggerFPR, biggerTPR) = bigger.head

//       val smallerDistance = fpr - smallerFPR
//       val biggerDistance = biggerFPR - fpr
      
//       val smallerWeight = biggerDistance / (biggerFPR - smallerFPR)
//       val biggerWeight = 1 - smallerWeight

//       smallerWeight * smallerTPR + biggerWeight * biggerTPR
//     }
//   }
// }

// object ROCData { 
//   def sorted(predictions: List[Double], truths: List[Boolean]): ROCData = { 
//     val (sortedPredictions, sortedTruths) = predictions.zip(truths).sortBy(_._1).unzip
//     ROCData(sortedPredictions, sortedTruths)
//   }

//   def fromExperiment(experiment: Experiment): List[ROCData] = {
//     if (!experiment.alreadyRun) throw new Exception("%s not run, results file should have the form %s".format(experiment, experiment.pathNoTime))
    
//     val resultPath = experiment.outDirectory + "/" + experiment.existingResultsFilename
//     val lines = io.Source.fromFile(resultPath).mkString.split("\n").filter(_.size > 0)
//     val foldCommentIndices = lines.zipWithIndex.filter(_._1.contains("Fold")).map(_._2)
//     for (i <- foldCommentIndices.toList) yield {
//       val distances = lines(i + 1).split(" ").map(_.toDouble)
//       val areSame = lines(i + 2).split(" ").map(_.toBoolean)
//       ROCData(distances.toList, areSame.toList)
//     }
//   }
// }


// case class ExperimentResults(val experiment: Experiment, val rocDataList: List[ROCData], recognitionRate: Double, val matchDistances: List[List[Double]], val impostorDistances: List[List[Double]]) { 
//   def print: String = {
//     val rocBlocks = for ((rocData, fold) <- rocDataList.zipWithIndex) yield {
//       val foldComment = "# Fold %s: distance, sameness, false positive rate, true positive rate".format(fold)
//       "%s\n%s".format(foldComment, rocData.prettyPrint)
//     }

//     val recognitionRateBlock = "# Recognition rate:\n%s".format(recognitionRate)

//     assert(matchDistances.size == impostorDistances.size)
//     val numTake = 2
//     val matchDistancesString = matchDistances.take(numTake).map(_.mkString(" ")).mkString("\n")
//     val impostorDistancesString = impostorDistances.take(numTake).map(_.mkString(" ")).mkString("\n")
//     val impostorBlock = "# Match and impostor distances:\n%s\n%s".format(matchDistancesString, impostorDistancesString)

//     val meanROCBlock = { 
//       val comment = "# Mean ROC data: false positive rate, true positive rate"
//       val (fprs, tprs) = meanROCCurve.unzip
//       "%s\n%s\n%s".format(comment, Util.truncate(fprs), Util.truncate(tprs))
//     }

//     "%s\n\n%s\n\n%s\n\n%s\n\n%s".format(experiment.prettyPrint, rocBlocks.mkString("\n\n"), recognitionRateBlock, impostorBlock, meanROCBlock)
//   }

//   def write { 
//     val file = new File(experiment.outPath)
//     val outString = print
//     println("writing: %s".format(file.toString))
//     // val out = new io.PrintWriter(file)
//     // out.print(print)
//     // out.close()
//     org.apache.commons.io.FileUtils.writeStringToFile(file, outString)
//     println("done writing")
//   }

//   def meanROCCurve = { 
//     val allFPRs = rocDataList.map(_.curve).flatten.map(_._1).distinct.sorted
    
//     def meanTPR(fpr: Double) = { 
//       rocDataList.map(_.tprFromFPR(fpr)).sum / rocDataList.size
//     }

//     val allTPRs = allFPRs.map(f => meanTPR(f))
//     allFPRs.zip(allTPRs)
//   }
// }

// case class MeanExperimentResults(val experiment: Experiment, val falsePositiveRate: List[Double], val truePositiveRate: List[Double]) 

// object MeanExperimentResults {
//   def fromCompletedExperiment(experiment: Experiment): MeanExperimentResults = {
//     if (!experiment.alreadyRun) throw new Exception("%s not run, results file should have the form %s".format(experiment, experiment.pathNoTime))

//     val resultPath = experiment.outDirectory + "/" + experiment.existingResultsFilename

//     val lines = io.Source.fromFile(resultPath).mkString.split("\n").filter(_.size > 0)
//     val meanLines = lines.dropWhile(!_.contains("Mean ROC data:"))

//     assert(meanLines.size == 3)
//     val falsePositiveRate = meanLines(1).split(" ").map(_.toDouble).toList
//     val truePositiveRate = meanLines(2).split(" ").map(_.toDouble).toList
//     MeanExperimentResults(experiment, falsePositiveRate, truePositiveRate)
//   }

//   def drawMeanROC(results: List[MeanExperimentResults]) {
//     val experiments = results.map(_.experiment)

//     // Drop the first value, the time the experiment was performed.
//     val parameterSets = experiments.map(_.filenameParts).transpose.map(_.toSet).tail

//     def setToString(set: Set[String]) = set.toList.mkString("-")

//     val title = parameterSets.map(setToString).mkString(" ")

//     val variableParameters = parameterSets.map(_.size > 1)
    
//     def curveName(experiment: Experiment): String = {
//       // val nameValuePairs = experiment.parameterAbbreviations.zip(experiment.filenameParts).tail
//       // val namedParameters = nameValuePairs.map({case (n, v) => n + ":" + v})
//       val namedParameters = experiment.filenameParts.tail

//       // We only care about parameters which vary.
//       val name = variableParameters.zip(namedParameters).filter(_._1).map(_._2).mkString(" ")
//       if (name.size > 0) name else "no comparison"
//     }

//     def cleanCurve(xs: List[Double], ys: List[Double]): Tuple2[List[Double], List[Double]] = {
//       assert(xs.size == ys.size)
//       val xChanged = (0 until xs.size).map(i => i == 0 || xs(i - 1) != xs(i))
//       val newXs = xChanged.zip(xs).filter(_._1).map(_._2).toList
//       val newYs = xChanged.zip(ys).filter(_._1).map(_._2).toList
//       (newXs, newYs)
//     }

//     def curveInfo(result: MeanExperimentResults) = {
//       val name = curveName(result.experiment)
//       val (fprs, tprs) = cleanCurve(result.falsePositiveRate, result.truePositiveRate)      
//       val fprString = fprs.mkString(" ")
//       val tprString = tprs.mkString(" ")
//       List(name, fprString, tprString).mkString("\n")
//     }

//     val tempContents = (List(title) ++ results.map(curveInfo)).mkString("\n")
//     val tempFile = File.createTempFile("rocdata", ".txt")
//     if (Global.run.deleteTemporaryFiles) tempFile.deleteOnExit
//     org.apache.commons.io.FileUtils.writeStringToFile(tempFile, tempContents)

//     val filename = title.replace(" ", "_") + ".png"
//     val outPath = "%s/results/roc_curves/%s".format(Global.run.projectRoot, filename)

//     val pythonScript = Global.run.projectRoot + "/python/single_roc.py"
//     val command = "python %s %s %s".format(pythonScript, tempFile, outPath)
//     Util.runSystemCommand(command)

//     println("wrote %s".format(outPath))
//   }
// }

// object DistanceHistogram {
//   def sameAndDifferentDistances(experiment: Experiment) = {
//     val (distances, areSame) = { 
//       val rocDataList = ROCData.fromExperiment(experiment)
//       assert(rocDataList.size == 1)
//       (rocDataList(0).predictions, rocDataList(0).truths)
//     }
    
//     val sameDistances = distances.zip(areSame).filter(_._2).map(_._1)
//     val differentDistances = distances.zip(areSame).filter(!_._2).map(_._1)
//     (sameDistances, differentDistances)
//   }

//   def drawDistanceHistogram(experiment: Experiment) {

//     // Drop the first value, the time the experiment was performed.
//     val title = experiment.filenameParts.tail.mkString(" ")

//     val (sameDistances, differentDistances) = sameAndDifferentDistances(experiment)

//     val tempContents = "%s\n%s\n%s".format(title, sameDistances.mkString(" "), differentDistances.mkString(" "))
//     val tempFile = Util.createTempFile("histogramdata", ".txt")
//     org.apache.commons.io.FileUtils.writeStringToFile(tempFile, tempContents)

//     val filename = title.replace(" ", "_") + ".png"
//     val outPath = "%s/results/histograms/%s".format(Global.run.projectRoot, filename)

//     val pythonScript = Global.run.childPath("python/distance_histogram.py")
//     val command = "python %s %s %s".format(pythonScript, tempFile, outPath)
//     Util.runSystemCommand(command)

//     println("wrote %s".format(outPath))
//   }
// }

// object ThreeDistanceHistogram {
//   def draw(l0: Experiment, cayley: Experiment, kt: Experiment) {
//     // Drop the first value, the time the experiment was performed.
//     val title = l0.filenameParts.tail.mkString(" ")

//     val (l0Same, l0Different) = DistanceHistogram.sameAndDifferentDistances(l0)
//     val (cayleySame, cayleyDifferent) = DistanceHistogram.sameAndDifferentDistances(cayley)
//     val (ktSame, ktDifferent) = DistanceHistogram.sameAndDifferentDistances(kt)

//     val tempContents = "%s\n%s".format(title, List(l0Same, l0Different, cayleySame, cayleyDifferent, ktSame, ktDifferent).map(_.mkString(" ")).mkString("\n"))
//     val tempFile = Util.createTempFile("threehistogramdata", ".txt")
//     org.apache.commons.io.FileUtils.writeStringToFile(tempFile, tempContents)

//     val filename = title.replace(" ", "_") + ".png"
//     val outPath = "%s/results/three_histograms/%s".format(Global.run.projectRoot, filename)

//     val pythonScript = Global.run.childPath("python/three_distance_histogram.py")
//     val command = "python %s %s %s".format(pythonScript, tempFile, outPath)
//     Util.runSystemCommand(command)

//     println("wrote %s".format(outPath))
//   }
// }

// object ImpostorHistogram {
//   def fromExperiment(experiment: Experiment): List[List[Double]] = {
//     if (!experiment.alreadyRun) throw new Exception("%s not run, results file should have the form %s".format(experiment, experiment.pathNoTime))
    
//     val resultPath = experiment.outDirectory + "/" + experiment.existingResultsFilename
//     val lines = io.Source.fromFile(resultPath).mkString.split("\n").toList
//     val foldCommentIndex = lines.zipWithIndex.filter(_._1.contains("Match and impostor")).map(_._2).head
//     // TODO: Assume 4 lines
//     val dataLines = for (i <- (foldCommentIndex + 1 until foldCommentIndex + 5).toList) yield lines(i)
//     println(resultPath)
// //    println(dataLines)
//     dataLines.map(_.split(" ").map(_.toDouble).toList)
//   }

//   def draw(experiment: Experiment) {
//     // Drop the first value, the time the experiment was performed.
//     val title = experiment.filenameParts.tail.mkString(" ")

//     val matchAndImpostorDistancesString = fromExperiment(experiment).map(_.mkString(" ")).mkString("\n")

//     val tempContents = "%s\n%s".format(title, matchAndImpostorDistancesString)
//     val tempFile = Util.createTempFile("impostordata", ".txt")
//     org.apache.commons.io.FileUtils.writeStringToFile(tempFile, tempContents)

//     val filename = title.replace(" ", "_") + ".png"
//     val outPath = "%s/results/impostors/%s".format(Global.run.projectRoot, filename)

//     val pythonScript = Global.run.childPath("python/impostor_histogram.py")
//     val command = "python %s %s %s".format(pythonScript, tempFile, outPath)
//     Util.runSystemCommand(command)

//     println("wrote %s".format(outPath))
//   }
// }

// object RecognitionRate {
//   def fromExperiment(experiment: Experiment): List[Double] = {
//     if (!experiment.alreadyRun) throw new Exception("%s not run, results file should have the form %s".format(experiment, experiment.pathNoTime))
    
//     val resultPath = experiment.outDirectory + "/" + experiment.existingResultsFilename
//     val lines = io.Source.fromFile(resultPath).mkString.split("\n").filter(_.size > 0)
//     val foldCommentIndices = lines.zipWithIndex.filter(_._1.contains("Recognition")).map(_._2)
//     for (i <- foldCommentIndices.toList) yield {
//       lines(i + 1).toDouble
//     }
//   }
// }

// object SurfacePlot {
//   def draw(experiments: List[Experiment]) = {
//     // Drop the first value, the time the experiment was performed.
//     val title = experiments(0).filenameParts.tail.mkString(" ")

//     val (blurs, sizes, rates) = (for (experiment <- experiments) yield {
//       assert(experiment.descriptor == "LUCID")
//       val blur = experiment.descriptorCommonArgs.blurWidth
//       val patchSize = experiment.descriptorExtraArgs(0).toInt
//       val recognitionRate = RecognitionRate.fromExperiment(experiment).head
//       (blur, patchSize, recognitionRate)
//     }).unzip3
    

//     val tempContents = "%s\n%s\n%s\n%s".format(title, blurs.mkString(" "), sizes.mkString(" "), rates.mkString(" "))
//     val tempFile = Util.createTempFile("surfaceplotdata", ".txt")
//     org.apache.commons.io.FileUtils.writeStringToFile(tempFile, tempContents)

//     val filename = title.replace(" ", "_") + ".png"
//     val outPath = "%s/results/surface_plots/%s".format(Global.run.projectRoot, filename)

//     val pythonScript = Global.run.childPath("python/surface_plot.py")
//     val command = "python %s %s %s".format(pythonScript, tempFile, outPath)
//     Util.runSystemCommand(command)

//     println("wrote %s".format(outPath))
//   }
// }
