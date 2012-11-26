#include <jni.h>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <vector>

#include "lucid/descriptors/elucid_binary_descriptor_extractor.h"
#include "lucid/descriptors/elucid_descriptor_extractor.h"

#include <sstream>
#include <iomanip>

using namespace std;

enum Method {
    LUCID_RANK, LUCID_BINARY_WIDE, ORB, BRISK
};

extern "C" {
JNIEXPORT void JNICALL Java_org_opencv_samples_tutorial3_Sample3View_FindFeatures(
        JNIEnv* env, jobject, jint doInitialize, jint do_ransac, jint width,
        jint height, jint mode, jbyteArray yuv, jintArray bgra) {
    jbyte* _yuv = env->GetByteArrayElements(yuv, 0);
    jint* _bgra = env->GetIntArrayElements(bgra, 0);

    cv::Mat myuv(height + height / 2, width, CV_8UC1, (unsigned char *) _yuv);
    cv::Mat mbgra(height, width, CV_8UC4, (unsigned char *) _bgra);
    cv::Mat mgray(height, width, CV_8UC1, (unsigned char *) _yuv);

    //Please make attention about BGRA byte order
    //ARGB stored in java as int array becomes BGRA at native level
    cv::cvtColor(myuv, mbgra, CV_YUV420sp2BGR, 4);

    std::vector<cv::KeyPoint> keypoints;

    // Figure out the features
    //FastFeatureDetector detector(50);
    static size_t n_features = 200;
    cv::OrbFeatureDetector detector(n_features, 1, 1, 50);
    //BriskFeatureDetector detector(100, 1, 1, 50);

    detector.detect(mgray, keypoints);

    for (size_t i = 0; i < keypoints.size(); i++)
        cv::circle(mbgra, keypoints[i].pt, 10, cv::Scalar(0, 0, 255, 255));

    // Figure out the descriptors
    std::vector<bool> valid_descriptors;
    cv::Mat descriptors;
    // Create a Lucid extractor with rotation invariance
    static lucid::DescriptorExtractor * descriptor_computer = 0;
    static int threshold;
    static int threshold_step;

    static Method method = LUCID_RANK;

    // Increase the threshold
    if (mode == 1)
        threshold += threshold_step;
    // Decrease the threshold
    else if ((mode == 2) && (threshold > 0))
        threshold -= threshold_step;

    // Do Lucid RANK or Binary
    else if ((mode == 4) || (mode == 5) || (mode == 6) || (mode == 7)) {
        doInitialize = true;
    }

    switch (mode) {
    case 4:
        method = LUCID_RANK;
        break;
    case 5:
        method = LUCID_BINARY_WIDE;
        break;
    case 6:
        method = ORB;
        break;
    case 7:
        method = BRISK;
        break;
    }

//    static bool is_first = true;
//    if (is_first) {
//        doInitialize = true;
//        is_first = false;
//    }

    // Create a descriptor_computer if we change mode or if we Initialize
    static bool is_initialized = false;
    if (doInitialize) {
        is_initialized = true;

        if (descriptor_computer)
            delete descriptor_computer;
        switch (method) {
        case LUCID_RANK:
            threshold = 150;
            threshold_step = 50;
            descriptor_computer =
                    static_cast<lucid::DescriptorExtractor*>(new lucid::ELucidDescriptorExtractor(
                            true, false));
            break;
        case LUCID_BINARY_WIDE:
            threshold = 40;
            threshold_step = 5;
            descriptor_computer =
                    static_cast<lucid::DescriptorExtractor*>(new lucid::ELucidBinaryDescriptorExtractor(
                            true, true, false));
            break;
        case ORB:
        case BRISK:
            threshold = 40;
            threshold_step = 5;
            break;
        }
    }

    static cv::Mat gray_ori;
    static cv::Mat bgra_ori;
    static cv::Mat descriptors_ori;
    static std::vector<bool> valid_test_descriptors;
    static std::vector<cv::KeyPoint> keypoints_train;

    std::vector<cv::DMatch> matches;
    if ((!keypoints.empty()) && is_initialized) {
        switch (method) {
        case LUCID_RANK:
        case LUCID_BINARY_WIDE:
            descriptor_computer->computeDescriptors(mgray, keypoints,
                    &valid_descriptors, &descriptors);
            break;
        case ORB: {
            cv::Ptr<cv::Feature2D::DescriptorExtractor> descriptor_extractor =
                    cv::Feature2D::DescriptorExtractor::create("ORB");
            descriptor_extractor->compute(mgray, keypoints, descriptors);
            break;
        }
        case BRISK: {
            cv::Ptr<cv::Feature2D::DescriptorExtractor> descriptor_extractor =
                    cv::Feature2D::DescriptorExtractor::create("BRISK");
            descriptor_extractor->compute(mgray, keypoints, descriptors);
            break;
        }
        }

        // Memorize the keypoints/descriptors for later comparisons
        if (doInitialize) {
            mgray.copyTo(gray_ori);
            mbgra.copyTo(bgra_ori);
            descriptors.copyTo(descriptors_ori);
            valid_test_descriptors = valid_descriptors;
            keypoints_train = keypoints;
        } else {
            if ((method == LUCID_RANK) || (method == LUCID_BINARY_WIDE)) {
                descriptor_computer->matchDescriptors(descriptors,
                        descriptors_ori, valid_test_descriptors,
                        valid_descriptors, &matches);
            }
            if ((method == ORB) || (method == BRISK)) {
                cv::Ptr<cv::DescriptorMatcher> matcher =
                        cv::DescriptorMatcher::create("BruteForce-Hamming");
                matcher->match(descriptors, descriptors_ori, matches);
            }
        }
    }

    // Clean matches based on distance matches
    std::vector<cv::Point2d> points_src;
    points_src.reserve(matches.size());
    std::vector<cv::Point2d> points_dst;
    points_dst.reserve(matches.size());
    for (size_t j = 0; j < matches.size(); ++j) {
        if (matches[j].distance > threshold)
            continue;
        points_dst.push_back(keypoints[matches[j].queryIdx].pt);
        points_src.push_back(keypoints_train[matches[j].trainIdx].pt);
    }
    cv::Mat mask = cv::Mat::ones(cv::Size(1, points_dst.size()), CV_8U);
    // Do RANSAC to filter if asked for
    if ((!points_dst.empty()) && (do_ransac)) {
        cv::Mat homography;
        int n_inliers = 6;
        if ((points_src.size() > n_inliers)
                && (points_dst.size() > n_inliers)) {
            // Perform RANSAC
            homography = findHomography(points_src, points_dst, CV_RANSAC, 8,
                    mask);
        }
    }

    // Draw matches
    size_t n_inliers = 0;
    size_t n_matches = points_dst.size();
    for (size_t j = 0; j < points_dst.size(); ++j) {
        cv::Scalar color;
        if (mask.at<uchar>(j)) {
            color = cv::Scalar(0, 255, 0, 255);
            ++n_inliers;
        } else
            color = cv::Scalar(0, 0, 255, 255);

        cv::line(mbgra, points_dst[j], points_src[j], color, 4);
    }

    // Add some information : threshold, % matches, % inliers
//    {
//        std::stringstream ss;
//        ss << threshold << "-" << (n_matches * 100) / n_features << "-"
//                << (n_inliers * 100) / n_features << "-"
//                << (n_inliers * 100) / std::max(n_matches, size_t(1));
//        cv::putText(mbgra, ss.str(), cv::Point(90, 160), cv::FONT_HERSHEY_PLAIN,
//                4.0, cv::Scalar(255, 255, 0, 255), 3);
//    }
    {
        std::stringstream ss;
        ss << std::setfill('0') << std::setw(2) << n_matches << "<thres";
        cv::putText(mbgra, ss.str(), cv::Point(90, 160), cv::FONT_HERSHEY_PLAIN,
                4.0, cv::Scalar(255, 255, 0, 255), 3);
    }
    {
        std::stringstream ss;
        ss << std::setfill('0') << std::setw(2)
                << (n_inliers * 100) / std::max(n_matches, size_t(1))
                << "%correct";
        cv::putText(mbgra, ss.str(), cv::Point(90, 240), cv::FONT_HERSHEY_PLAIN,
                4.0, cv::Scalar(255, 255, 0, 255), 3);
    }

    {
        std::stringstream ss;
        switch (method) {
        case LUCID_RANK:
            ss << "eLUCID RANK";
            break;
        case LUCID_BINARY_WIDE:
            ss << "eLUCID BINARY";
            break;
        case ORB:
            ss << "ORB";
            break;
        case BRISK:
            ss << "BRISK";
            break;
        }
        cv::putText(mbgra, ss.str(), cv::Point(90, 80), cv::FONT_HERSHEY_PLAIN,
                4.0, cv::Scalar(255, 255, 0, 255), 3);
    }

    env->ReleaseIntArrayElements(bgra, _bgra, 0);
    env->ReleaseByteArrayElements(yuv, _yuv, 0);
}

}
