import java.io.File
import com.frugalmechanic.optparse.{ MultiStrOpt, OptParse, OptValToOption, StrOpt }
import nebula.{ CorrespondenceExperiment, CorrespondenceExperimentResults, Global, Histogram, IO, RuntimeConfig, TableUnrendered, Util }
import nebula.Summary

object RunExperiments extends OptParse {
  def mkTable(experiments: Seq[Seq[CorrespondenceExperiment]]) {
    // Run the experiments, in parallel if desired.
    // TODO: This will load each experiment twice, which is inelegant.
    Util.parallelize(experiments.flatten).foreach(runExperiment)

    val table = TableUnrendered(experiments)

    println("Table title is %s".format(table.title))
    println("Writing table to %s".format(table.path))
    val tsv = table.toTSV(x => TableUnrendered.recognitionRate(x).formatted("%.2f"))
    org.apache.commons.io.FileUtils.writeStringToFile(table.path, tsv)
  }

  def runExperiment(experiment: CorrespondenceExperiment) {
    // TODO: Sort out the |skipCompletedExperiments| logic.
    if (Global.run[RuntimeConfig].skipCompletedExperiments && experiment.alreadyRun) {
      println("Already run, skipping: %s".format(experiment))
      return
    } else {
      println("Running experiment: %s".format(experiment))
    }

    val results = CorrespondenceExperimentResults.runExperiment(experiment)

    println("Recognition rate was %s".format(Summary.recognitionRate(results.dmatches)))
  }

  val runtimeConfigFile = StrOpt()
  val experimentConfigFilesOrDirectory = MultiStrOpt()
  val tableConfigFiles = MultiStrOpt()

  def main(args: Array[String]) {
    parse(args)

    RuntimeConfig.init(new File(runtimeConfigFile.get))

    if (experimentConfigFilesOrDirectory.isDefined) {
      val experimentConfigFiles: Seq[File] = {
        val paths = experimentConfigFilesOrDirectory.get
        val files = paths.map(path => new File(path)).toSeq
        files match {
          case Seq(file) =>
            if (file.isDirectory) file.listFiles.toList
            else files
          case _ => files
        }
      }

      def loadExperiments(file: File): Seq[CorrespondenceExperiment] = {
        // Guess the data format by the filename.
        val oneDimensional = file.getName.contains("experiment")
        val twoDimensional = file.getName.contains("table")
        require(oneDimensional ^ twoDimensional)

        if (oneDimensional) IO.interpretFile[Seq[CorrespondenceExperiment]](file)
        else IO.interpretFile[Seq[Seq[CorrespondenceExperiment]]](file).flatten
      }

      val experiments: Seq[CorrespondenceExperiment] = experimentConfigFiles.flatMap(loadExperiments)

      println("There are %s experiments:".format(experiments.size))
      experiments.foreach(println)

      Util.parallelize(experiments).foreach(runExperiment)      

      println("All experiments completed.")
    }

        if (tableConfigFiles.isDefined) {
          val files = tableConfigFiles.get.map(name => new File(name))
          val tables = files.map(IO.interpretFile[Seq[Seq[CorrespondenceExperiment]]])
          tables.foreach(mkTable)
        }
  }
}
