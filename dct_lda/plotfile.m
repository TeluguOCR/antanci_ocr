h = figure('Visible', 'off');

t = 1:.01:2*pi;
A = zeros(200);
x = floor(100*sin(t)+101);
y = floor(100*cos(t)+101);
i = sub2ind(size(A), x, y);
A(i) = 255;
imshow(A);

saveas(h, 'image.png');