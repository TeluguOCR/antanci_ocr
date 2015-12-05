% Telugu OCR Project
% - This file opens the glyph for each character (multiple times)
% - Then it opens the new Canonical variate featureimages
% - Finds the features for each character as the correlation between each
%    character image and CV image.
% - Finds the inclass cov matrix (W)
% - Class centers (M)
% - Stores the W^-.5 and W^-.5 * M matrices for use by Banti
pkg load image

IDX = 1; LEFT = 2; BOT = 3; RIGHT = 4; TOP= 5; BASE = 6;

STD_SZ = 64;            % Standard scaling size
file_names = ['p385'; 'g385'; 'r385'];
nFonts = size(file_names, 1);

DUPS = 50;
NOISE_LEVEL = 0.1;
ROT_MAX = 6;

i_row = 0;
data = [];
nChars = 0;

for iFont = 1:nFonts
  fprintf('\n%s :    ', file_names(iFont,:));
  img_full = imread([file_names(iFont,:) '.tif']);  % Open Image file
  coords =  spcread([file_names(iFont,:) '.box'], 6);  % Open Box file
  [H ~] = size(img_full);
  coords(:, [BOT TOP]) = H - coords(:, [BOT TOP]); % Correct Y co-ord
  coords(:,2:end) = coords(:,2:end) + 1;           % C to Matlab index
  nBoxes = size(coords, 1);
  %nBoxes = 100;    % Enable for Test Run
  data = [data; zeros(nBoxes*DUPS, 1+STD_SZ*STD_SZ)];
  nChars = max([nChars; coords(1:nBoxes,IDX)]);

  for iBox = 1:nBoxes
    fprintf('\b\b\b%3d', iBox);
    neg_imgn = 1-img_full(coords(iBox, TOP) :coords(iBox,BOT), ...
						  coords(iBox, LEFT):coords(iBox,RIGHT));
    charID = coords(iBox, IDX);
    % Noise each file required number of times
    for idup = [1:DUPS]
        imgnois = neg_imgn .* (rand(size(neg_imgn)) > NOISE_LEVEL);
        imgnois = 1-imrotate(imgnois, 2*ROT_MAX*rand()-ROT_MAX);
        imgnois(isna(imgnois)) = 1;
        aspect_ratio = size(imgnois, 1) / size(imgnois, 2);
        imgnois = imresize(imgnois, [STD_SZ STD_SZ], 'nearest');
        % Calculate Features via inner product with CV pics
        i_row = i_row + 1;
        data(i_row,:) = [charID reshape(dct2(imgnois), [1 STD_SZ*STD_SZ])];
    end
  end
end

fprintf('\nGenerating the center & variance for class:    ');
M = zeros(nChars, STD_SZ*STD_SZ);        % Class Centers
W = zeros(STD_SZ*STD_SZ);                % In Class Covariance
for i = 1:nChars
    fprintf('\b\b\b%3d', i);
    data_i = data(logical(data(:,1)==i), 2:end);
    nSamples = size(data_i,1);
    if nSamples == 0
        fprintf('\nCharacter Class %d not found in range 1 to %d', i, nChars);
        fprintf('\nThis is like Shooting yourself in the foot');
    end
    M(i, :) = mean(data_i);
    meanremoved = data_i - ones(nSamples, 1) * M(i, :);
    W = W + cov(meanremoved);
end
W = W / nChars;

fprintf('\nFinding W^(-1/2)');
[U D ~] = svd(W);
fprintf('\nSVD Done')
lambda = 1;
Wneghalf = U * diag(diag(D+lambda) .^ -.5);    % W ^ -0.5
B = cov(M);  
Bstar = Wneghalf' * B * Wneghalf;
[Vstar Dstar ~] = svd(Bstar);

sqrtnCV = 20;
nCVs = sqrtnCV^2;
V = Wneghalf * Vstar(:, 1:nCVs);
V = reshape(V, [STD_SZ STD_SZ nCVs]);
Vimg = zeros(size(V));
for iCV = 1:nCVs
  Vimg(:,:,iCV) = idct2(V(:,:,iCV));
end

GAP = 1;
plt = zeros((GAP+STD_SZ)*sqrtnCV-GAP);
for jPic = 1:sqrtnCV
  for iPic = 1:sqrtnCV
    i = sub2ind([sqrtnCV sqrtnCV], iPic, jPic);
    x = (iPic-1)*(STD_SZ+GAP)+1;
    y = (jPic-1)*(STD_SZ+GAP)+1;
    plt(x:x+STD_SZ-1, y:y+STD_SZ-1) = mat2gray(Vimg(:,:, i));
  end
end

imwrite(plt, 'basis.png', 'png');

fprintf('\nSaving\n');
fdctbasis = fopen('basis.bin', 'w');
fwrite(fdctbasis, Vimg, 'double');    
fclose(fdctbasis);
