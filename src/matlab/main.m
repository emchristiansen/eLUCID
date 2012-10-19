clear;

import datasets.*;
import benchmarks.*;
import localFeatures.*;

% -------------------------------------------------------------------------

sift = VlFeatSift();

mser = VlFeatMser();
mserWithSift = DescriptorAdapter(mser, sift);

lucidRadial = DescriptorAdapter(VlFeatSift(), LUCID('pattern', 'radial'));
lucidBlock = DescriptorAdapter(VlFeatSift(), LUCID('pattern', 'block'));
lucidSteer = DescriptorAdapter(VlFeatSift(), ...
  LUCID('pattern', 'radial', 'steerScale', false, 'steerRotation', true));

% featExtractors = {siftDetector, mserWithSift, RandomFeaturesGenerator()};
% featExtractors = {ExampleLocalFeatureExtractor(), lucid, mserWithSift};
featExtractors = {lucidSteer, lucidRadial};
distances = {'L1', 'L1'};

% -------------------------------------------------------------------------

% categoryNames = {'graf','wall','boat','bark','bikes','trees',...
%       'ubc','leuven'};
    
categoryNames = {'boat'};    

% dataset = datasets.VggAffineDataset('Category','bikes');

% -------------------------------------------------------------------------

matchScore = [];
numMatches = [];

for s = 1:numel(categoryNames)
  dataset = datasets.VggAffineDataset('Category',categoryNames{s});
  fprintf('category is %s\n', categoryNames{s});
  for d = 1:numel(featExtractors)
    matchingBenchmark = RepeatabilityBenchmark(...
      'Mode','MatchingScore',...
      'descriptorsDistanceMetric', distances{d});
    % matchingBenchmark.disableCaching();
    matchingBenchmark.Opts.descriptorsDistanceMetric      
      
    for i = 2:dataset.NumImages
      [matchScore(s, d,i) numMatches(s, d,i)] = ...
        matchingBenchmark.testFeatureExtractor(featExtractors{d}, ...
                                dataset.getTransformation(i), ...
                                dataset.getImagePath(1), ...
                                dataset.getImagePath(i));
    end
    
    matchScore(s, d, :)
  end
end

matchScore


