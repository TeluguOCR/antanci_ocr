SpeheredM = Wneghalf * M';              % Each mean is a column

% Matlab dumps last element wise (i.e. column wise in a 2D matrix)
if(0)
  fprintf('\nSaving\n');
  fWnegHalf = fopen('neg_sqroot_cov.bin', 'w');
  fwrite(fWnegHalf, Wneghalf, 'double');
  fclose(fWnegHalf);

  fSpeheredM = fopen('sphered_means.bin', 'w');
  fwrite(fSpeheredM, SpeheredM, 'double');
  fclose(fSpeheredM);
end

fprintf('\n');