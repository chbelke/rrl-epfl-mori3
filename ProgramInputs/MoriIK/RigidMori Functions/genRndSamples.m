function rndSamples = genRndSamples(maxStep, currentVal, maxVal, minVal, n)
%GENRNDSAMPLES Generates n random samples for an actuator position
% within step and actuator limits, guaranteed to include curent, max and min value 
    upperBound = min(maxVal, currentVal+maxStep);
    lowerBound = max(minVal, currentVal-maxStep);
    rndSamples = [currentVal;(upperBound-lowerBound)*[0;1;rand(n-3,1)]+lowerBound*ones(n-1,1)];
end

