function pattern = rotatePattern(angle)
unrotated = csvread('pattern_49x49.csv');
pattern = zeros(size(unrotated));

for i = 1 : size(pattern, 2)
  % Rotate about the patch center.
  yAbs = unrotated(1, i);
  xAbs = unrotated(2, i);
  yRel = yAbs - 25;
  xRel = xAbs - 25;
  vNorm = norm([xRel yRel]);
  vAngle = atan2(yRel, xRel);
  newAngle = vAngle + angle; % Also try subtracting angle.
  xNew = round(vNorm * cos(newAngle)) + 25;
  yNew = round(vNorm * sin(newAngle)) + 25;
  pattern(:, i) = [yNew xNew];
end
end