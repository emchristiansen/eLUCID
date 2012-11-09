// Author: Andrew Ziegler <andrewzieg@gmail.com>

#include "lucid/tools/experiment_driver.h"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>

#include "lucid/tools/util.h"
#include "lucid/tools/experiment_results.h"
#include "lucid/detectors/fast_feature_detector.h"
#include "lucid/detectors/harris_feature_detector.h"
#include "lucid/detectors/brisk_feature_detector.h"
#include "lucid/detectors/sift_feature_detector.h"
#include "lucid/detectors/censure_feature_detector.h"
#include "lucid/detectors/surf_feature_detector.h"
#include "lucid/descriptors/lucid_descriptor_extractor.h"
#include "lucid/descriptors/elucid_descriptor_extractor.h"
#include "lucid/descriptors/elucid_binary_descriptor_extractor.h"
#include "lucid/descriptors/brief_descriptor_extractor.h"
#include "lucid/descriptors/brisk_descriptor_extractor.h"
#include "lucid/descriptors/surf_descriptor_extractor.h"
#include "lucid/descriptors/sift_descriptor_extractor.h"
#include "lucid/descriptors/orb_descriptor_extractor.h"


#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <assert.h>
#include <cmath>

namespace lucid
{
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

  void ExperimentDriver::rotationMatchingExperiment(
    const int max_features,
    const float radius,
    const std::vector<float>& angles,
    const cv::Mat& reference_image,
    const cv::Mat& test_image,
    const cv::Matx33f homography,
    const lucid::FeatureDetector& detector,
    const std::vector<lucid::DescriptorExtractor*>& extractors,
    std::vector<std::vector<float> > *recognition_rates)
  {
    // TODO: Port Experiment from Matlab
  }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

  void ExperimentDriver::detectionMatchingExperiment(
    const int max_features,
    const float radius,
    const cv::Mat& reference_image,
    const std::vector<cv::Mat>& test_images,
    const std::vector<cv::Matx33f> homographies,
    const lucid::FeatureDetector& detector,
    const std::vector<lucid::DescriptorExtractor*>& extractors,
    const std::string output_file_base)
  {
    // Detect features in first image.
    std::vector<cv::KeyPoint> orig_ref_features;
    detector.detectFeatures(reference_image,
                            max_features,
                            &orig_ref_features);

    // Detect features in other images.
    for(int i = 0; i < test_images.size(); ++i)
    {
      // We will use the smallest number of detected features in any
      // image as in "A performance evaluation of local descriptors"
      
      // Detect features other images.
      std::vector<cv::KeyPoint> orig_test_features;
      detector.detectFeatures(test_images[i],
                              max_features,
                              &orig_test_features);
      
      int num_features = std::min(orig_ref_features.size(),
                                  orig_test_features.size());

      // Get subvectors of features so that they all have the same number.
      // Make them const to prevent mutation during expermients.
      const std::vector<cv::KeyPoint>
        reference_features(orig_ref_features.begin(),
                           orig_ref_features.begin() + num_features);
      assert(reference_features.size() == num_features);    

      const std::vector<cv::KeyPoint>
        test_features(orig_test_features.begin(),
                      orig_test_features.begin() + num_features);
      assert(test_features.size() == num_features);

      // Find ground truth matches for each feature in the reference image
      // mapped into each of the test images. -1 if no match.
      std::vector<int> true_match_idxs(num_features, -1);

//      std::vector<cv::DMatch> true_matches;

      // Get homography.
      float h00 = homographies[i](0,0);
      float h01 = homographies[i](0,1);
      float h02 = homographies[i](0,2);
      float h10 = homographies[i](1,0);
      float h11 = homographies[i](1,1);
      float h12 = homographies[i](1,2);
      float h20 = homographies[i](2,0);
      float h21 = homographies[i](2,1);
      float h22 = homographies[i](2,2);

      for(int j = 0; j < num_features; ++j)
      {
        float ref_x = reference_features[j].pt.x;
        float ref_y = reference_features[j].pt.y;
        float warped_x = (h00 * ref_x) + (h01 * ref_y) + h02; 
        float warped_y = (h10 * ref_x) + (h11 * ref_y) + h12; 
        float w = (h20 * ref_x) + (h21 * ref_y) + h22; 
        warped_x /= w;
        warped_y /= w;
        
        for(int k = 0; k < num_features; ++k)
        {
          // Check for match.     
          float test_x = test_features[k].pt.x;
          float test_y = test_features[k].pt.y;

          float delta_x = warped_x - test_x;
          float delta_y = warped_y - test_y;
          float distance = sqrt(delta_x*delta_x + delta_y*delta_y);

          if(distance <= radius)
          {
            true_match_idxs[j] = k;

//            true_matches.push_back(cv::DMatch(j, k, 0));
            // Allow for only one true match. There would only be more
            // than one if there are multiple features within the radius.
            break;
          }
        }
      }

//      cv::Mat match_image;
      // Visualize Matches
      // cv::drawMatches(_reference_image,
      //                 reference_features,
      //                 _images[i],
      //                 test_features,
      //                 true_matches,
      //                 match_image);
      // cv::namedWindow("Ground Truth Matches", CV_WINDOW_AUTOSIZE);
      // cv::imshow("Ground Truth Matches", match_image);
      // cv::waitKey(0);
      
        
      // Match features with each registered descriptor extractor.
      for(int e = 0; e < extractors.size(); ++e)
      {
        const DescriptorExtractor& extractor = *extractors[e];
      
        cv::Mat reference_descs;
        std::vector<bool> valid_reference_descs;
        extractor.computeDescriptors(reference_image,
                                     reference_features,
                                     &valid_reference_descs,
                                     &reference_descs);
        assert(valid_reference_descs.size() == num_features);
        assert(reference_descs.rows == num_features);

        cv::Mat test_descs;
        std::vector<bool> valid_test_descs;
        extractor.computeDescriptors(test_images[i],
                                     test_features,
                                     &valid_test_descs,
                                     &test_descs);
        assert(valid_test_descs.size() == num_features);
        assert(test_descs.rows == num_features);
        
        std::vector<std::vector<cv::DMatch> > matches;
        // Match to everything.
        extractor.knnMatchDescriptors(num_features,
                                      test_descs,
                                      reference_descs,
                                      valid_test_descs,
                                      valid_reference_descs,
                                      &matches);

        std::fstream output_file;
        std::stringstream ss;
        ss << output_file_base 
           << "_"
           << "Image1_" << i+2
           << "_"
           << num_features
           << "_"
           << extractor.name() 
           << "_"
           << detector.name()
           << ".out";

        const char* file_name = ss.str().c_str();
        output_file.open(file_name, std::fstream::out);

        // Write distances to file and free up memory.
        for(int m = 0; m < matches.size(); ++m)
        {
          for(int k = 0; k < matches[m].size(); ++k)
          {
            int ref_idx = matches[m][k].trainIdx;
            int test_idx = matches[m][k].queryIdx;
            assert(valid_test_descs[test_idx]);
            assert(valid_reference_descs[ref_idx]);
            
            if(true_match_idxs[ref_idx] == test_idx)
            {
              output_file << "1 ";
            }
            else
            {
              output_file << "0 ";
            }
            output_file << matches[m][k].distance << std::endl;
          }
        }
        output_file.close();
      }
    }
  }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

  void ExperimentDriver::symmetricMatchingExperiment(
    double percent_non_matching,
    const int max_features,
    const cv::Mat& reference_image,
    const std::vector<cv::Mat>& test_images,
    const std::vector<cv::Matx33f> homographies,
    const lucid::FeatureDetector& detector,
    const std::vector<lucid::DescriptorExtractor*>& extractors,
    std::vector<std::vector<float> > *recognition_rates)
  {
    // Detect up to 3 times the number of features and select the
    // best such that they are not warped out of bounds.
    std::vector<cv::KeyPoint> temp_features;
    detector.detectFeatures(reference_image,
                            3*max_features,
                            &temp_features);

    // Make features const to prevent any mutation between experiments.
    const std::vector<cv::KeyPoint> orig_features = temp_features;

    std::vector<std::vector<cv::KeyPoint> > temp_test_features;
    std::vector<std::vector<cv::KeyPoint> > temp_reference_features;
    std::vector<std::vector<bool> > true_negatives;
    // Warp features into other images.
    for(int i = 0; i < homographies.size(); ++i)
    {
      std::vector<cv::KeyPoint> cur_test_features;
      std::vector<cv::KeyPoint> cur_ref_features;
      std::vector<bool> cur_true_negatives;
      for(int j = 0;
          j < orig_features.size() && cur_ref_features.size() < max_features;
          ++j)
      {
        // TODO: Use OpenCV's built in Matrix Multiplication.
        float h00 = homographies[i](0,0);
        float h01 = homographies[i](0,1);
        float h02 = homographies[i](0,2);
        float h10 = homographies[i](1,0);
        float h11 = homographies[i](1,1);
        float h12 = homographies[i](1,2);
        float h20 = homographies[i](2,0);
        float h21 = homographies[i](2,1);
        float h22 = homographies[i](2,2);

        float x = orig_features[j].pt.x;
        float y = orig_features[j].pt.y;

        float warped_x = (h00 * x) + (h01 * y) + h02; 
        float warped_y = (h10 * x) + (h11 * y) + h12; 
        float w = (h20 * x) + (h21 * y) + h22; 

        warped_x /= w;
        warped_y /= w;

        // Cull points that are warped out of bounds.
        if(warped_x > 0 &&
           warped_y > 0 &&
           test_images[i].cols > warped_x &&
           test_images[i].rows > warped_y)
        {
          cv::KeyPoint warped_feature(orig_features[j]);
          warped_feature.pt.x = warped_x;
          warped_feature.pt.y = warped_y;
          cur_test_features.push_back(warped_feature);
          cur_ref_features.push_back(orig_features[j]);
          cur_true_negatives.push_back(false);
        }
      }

      /* Choose random points to be non-matches*/
      std::vector<int> trueMatchIndices;
      std::vector<int> nonMatchIndices;

      // Initialize all indices.
      for(int m = 0; m < cur_test_features.size(); ++m)
      {
        trueMatchIndices.push_back(m);
      }

      // Select random matches to be true negatives.
      for(int m = 0; m < percent_non_matching * cur_test_features.size(); ++m)
      {
        int rand_idx = rand() % trueMatchIndices.size(); 
        nonMatchIndices.push_back(trueMatchIndices[rand_idx]);
        trueMatchIndices.erase(trueMatchIndices.begin() + rand_idx);
      }

      // Select random correspondences for each non-match.
      for(int m = 0; m < nonMatchIndices.size(); ++m)
      {
        int non_match_idx = nonMatchIndices[m];
        cur_test_features[non_match_idx].pt.x = rand() % test_images[i].cols;
        cur_test_features[non_match_idx].pt.y = rand() % test_images[i].rows;
        cur_true_negatives[non_match_idx] = true;
      }

      temp_test_features.push_back(cur_test_features);
      temp_reference_features.push_back(cur_ref_features);
      true_negatives.push_back(cur_true_negatives);
    }
    
    // Make features const to prevent any mutation between experiments.
    const std::vector<std::vector<cv::KeyPoint> >
      test_features = temp_test_features;
    const std::vector<std::vector<cv::KeyPoint> >
      reference_features = temp_reference_features;

    assert(test_features.size() == reference_features.size());
    
    // Match features with each registered descriptor extractor.
    for(int i = 0; i < extractors.size(); ++i)
    {
      // Make room for recognition rates for this extractor.
      recognition_rates->push_back(std::vector<float>());
      const DescriptorExtractor& extractor = *extractors[i];
      
      for(int j = 0; j < test_images.size(); ++j)
      {
        assert(test_features[j].size() == reference_features[j].size());
        int num_features = reference_features[j].size();

        cv::Mat reference_descs;
        std::vector<bool> valid_reference_descs;
        extractor.computeDescriptors(reference_image,
                                     reference_features[j],
                                     &valid_reference_descs,
                                     &reference_descs);
        cv::Mat test_descs;
        std::vector<bool> valid_test_descs;
        extractor.computeDescriptors(test_images[j],
                                     test_features[j],
                                     &valid_test_descs,
                                     &test_descs);

        std::vector<cv::DMatch> matches;
        extractor.matchDescriptors(test_descs,
                                   reference_descs,
                                   valid_test_descs,
                                   valid_reference_descs,
                                   &matches);

        std::cout << "matches.size() = " << matches.size() << std::endl;
        std::cout << "num_features = " << num_features << std::endl;

        uint true_matches = 0;
        uint valid_matches = 0;
        uint false_matches = 0;
        for(int m = 0; m < matches.size(); ++m)
        {
          if(valid_test_descs[m] && valid_reference_descs[m])
          {
            if((matches[m].queryIdx == matches[m].trainIdx) &&
               !true_negatives[j][matches[m].queryIdx]) 
            {
              ++true_matches;
            }
            else
            {
              ++false_matches;
            }
            ++valid_matches;
          }
        }

        assert((false_matches + true_matches) == valid_matches);
        assert(valid_matches <= num_features);
        
        float rec_rate = static_cast<float>(true_matches) / valid_matches;

      // NIPS metric.
//        float rec_rate = static_cast<float>(true_matches) / num_features;

      std::cout << "rec_rate = " << rec_rate << std::endl; 
      (*recognition_rates)[i].push_back(rec_rate);
      }
    }
  }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

} // namespace lucid

int main(int argc, char *argv[])
{
  if(argc < 3)
  {
    std::cout << "Not enough arguments" << std::endl;
    return 1;
  }

  lucid::ExperimentDriver *driver = NULL;

  lucid::FeatureDetector *feature_detector;

  double percent_non_matching = 0;
  // This is the value used in "A performance evaluation of local descriptors"
  float radius = 1.5;
  int lucid_window_size = 24;
  int num_features = 500;
  int multiplier = 3;
  // Default: < 0 "as is", =0 grayscale, > 0 rgb 
  bool cv_image_mode = 0;

  std::string base_file_name = "test";

  cv::Mat reference_image;
  std::vector<cv::Mat> images;
  std::vector<cv::Matx33f> homographies;
  std::vector<lucid::DescriptorExtractor*> extractors;  

  for(int i = 1; i < argc; ++i)
  {
    char flag = argv[i][1];
    switch(argv[i][0])
    {
    case '-':
      if('o' == flag)
      {
        base_file_name = std::string(argv[++i]);
        break;
      }
      else if('r' == flag)
      {
        std::stringstream ss(argv[++i], std::stringstream::in);        
        ss >> radius;
        break;
      }
      else if('p' == flag)
      {
        std::stringstream ss(argv[++i], std::stringstream::in);        
        ss >> percent_non_matching;
        break;
      }
      else if('g' == flag)
      {
        std::cout << "Setting image mode to RGB." << std::endl;
        cv_image_mode = -1;        
        break;
      }
      else if('n' == flag)
      {
        std::stringstream ss(argv[++i], std::stringstream::in);        
        ss >> num_features;
        break;
      } 
      else if('m' == flag)
      {
        std::stringstream ss(argv[++i], std::stringstream::in);        
        ss >> multiplier;
        break;
      } 
      else if('w' == flag)
      {
        std::stringstream ss(argv[++i], std::stringstream::in);        
        ss >> lucid_window_size;
        std::cout << "Using LUCID window size of "
                  << lucid_window_size
                  << std::endl;
        break;
      } 
      else if('d' == flag)
      {
        char *detector = argv[++i];

        switch(detector[0])
        {
        case 't':
        case 'T':
          std::cout << "Using SIFT Features" << std::endl;
          feature_detector = new lucid::SiftFeatureDetector();
          break;
        case 'c':
        case 'C':
          std::cout << "Using Censure Features" << std::endl;
          feature_detector = new lucid::CensureFeatureDetector(multiplier);
          break;
        case 's':
        case 'S':
          std::cout << "Using SURF Features" << std::endl;
          feature_detector = new lucid::SurfFeatureDetector(multiplier);
          break;
        case 'h':
        case 'H':
           std::cout << "Using Harris Features" << std::endl;
           feature_detector = new lucid::HarrisFeatureDetector(multiplier);
           break;
        case 'b':
        case 'B':
           std::cout << "Using BRISK Features" << std::endl;
           feature_detector = new lucid::BriskFeatureDetector();
           break;
        case 'f':
        case 'F':
        default:
          std::cout << "Using FAST Features" << std::endl;
          feature_detector = new lucid::FastFeatureDetector(multiplier);
          break;
        }
        
        break;
      }

      std::cout << "Flag not supported." << std::endl;
      return 1;
    default:
      int num_images = (argc - i + 1) / 2;
      int num_homogs = num_images - 1;
      if((num_images + num_homogs != argc - i) | num_images < 2)
      {
        std::cout << "Incorrect number of arguments" << std::endl;
        return 1;
      }
      else
      {
        // Get reference image.
        reference_image = cv::imread(argv[i++], cv_image_mode);
        if(reference_image.data == NULL)
        {
          std::cout << "Problem loading image file "
                    << argv[i-1]
                    << std::endl;
          return 1;
        }
        else
        {
          std::cout << "Loaded reference image from file "
                    << argv[i-1]
                    << std::endl;
        }

        // Get the rest of the images.
        for(int j = 0; j < num_images - 1; ++j)
        {
          cv::Mat test_image = cv::imread(argv[i++], cv_image_mode);
          if(test_image.data == NULL)
          {
            std::cout << "Problem loading image file "
                      << argv[i-1]
                      << std::endl;
            return 1;
          }
          else
          {
            std::cout << "Loaded image from file "
                      << argv[i-1]
                      << std::endl;
          }
          images.push_back(test_image);
        }

        for(int j = 0; j < num_homogs; ++j)
        {
          cv::Matx33f homog;
          if(!lucid::Util::readHomographyFromFile(argv[i++], &homog))
          {
            std::cout << "Problem loading homography "
                      << argv[i-1]
                      << std::endl;
            return 1;
          }
          else
          {
            std::cout << "Loaded homography from file "
                      << argv[i-1]
                      << std::endl;
            
            std::cout << std::endl
                      << cv::Mat(homog)
                      << std::endl;
          }
          homographies.push_back(homog);
        }
      }
    }
  }

  // lucid::ELucidDescriptorExtractor elucid_extractor(true);
  // extractors.push_back(&elucid_extractor);                     

  // lucid::ELucidBinaryDescriptorExtractor elucid_binary_extractor1(true);
  // extractors.push_back(&elucid_binary_extractor1);                     

  // lucid::ELucidBinaryDescriptorExtractor elucid_binary_extractor2(true);
  // extractors.push_back(&elucid_binary_extractor2);                     

  // lucid::LucidDescriptorExtractor lucid_extractor(lucid_window_size, 5);
  // extractors.push_back(&lucid_extractor);                     

  // lucid::BriefDescriptorExtractor brief_extractor;
  // extractors.push_back(&brief_extractor);

  // lucid::OrbDescriptorExtractor orb_extractor;
  // extractors.push_back(&orb_extractor);                     
  
  // lucid::BriskDescriptorExtractor brisk_extractor;
  // extractors.push_back(&brisk_extractor);                     
  
  // lucid::SurfDescriptorExtractor surf_extractor;
  // extractors.push_back(&surf_extractor);                     
  
  // lucid::SiftDescriptorExtractor sift_extractor;
  // extractors.push_back(&sift_extractor);                     

  // std::vector<std::vector<float> > recognition_rates;
  // lucid::ExperimentDriver::symmetricMatchingExperiment(percent_non_matching,
  //                                                      num_features,
  //                                                      reference_image,
  //                                                      images,
  //                                                      homographies,
  //                                                      *feature_detector,
  //                                                      extractors,
  //                                                      &recognition_rates);             
    
  // // Display results.
  // for(int i = 0; i < extractors.size(); ++i)
  // {

  //   std::cerr << feature_detector->name()
  //             << std::endl
  //             << extractors[i]->name()
  //             << " descriptors: "
  //             << std::endl;
  //   for(int j = 0; j < images.size(); j+=2)
  //   {      
  //     std::cerr << "Recognition rate = "
  //               << recognition_rates[i][j]
  //               << std::endl;
  //   }
  // }

  // std::cerr << std::endl << std::endl;


  delete feature_detector;
    
  return 0;
}
