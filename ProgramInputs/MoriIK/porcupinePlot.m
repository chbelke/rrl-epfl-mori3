function porcupinePlot(rbtree, results, startConfig)

samples = results.samples;
numSamples = size(samples, 3);

%Initialize figure
figure()
ax = show(rbtree, startConfig, 'Collisions','on','Visuals','off');
% Find all patch objects with the default collision mesh color
patches = ax.findobj('FaceColor',[0.2000 0.7000 0.2000]);
set(patches, 'FaceColor', 'b')
alpha(ax,0.2); %Set transparency
hold on

%xlim([-0.5 1])

for lv = 1:numSamples
    if results.residuals(lv)~=0 %If the algorithm didn't converge within maxIter
        % Add element to plot
        plot3(samples(1,2,lv),samples(2,2,lv),samples(3,2,lv),'rx');
        quiver3(samples(1,2,lv),samples(2,2,lv),samples(3,2,lv),samples(1,1,lv),samples(2,1,lv),samples(3,1,lv),'r:');
        % TODO Plot final position?
    else
        % Add element to plot
        plot3(samples(1,2,lv),samples(2,2,lv),samples(3,2,lv),'gx');
        quiver3(samples(1,2,lv),samples(2,2,lv),samples(3,2,lv),samples(1,1,lv),samples(2,1,lv),samples(3,1,lv),'g:');
    end
end


%Produce a plot showing successful targets, failed targets and final position
grid on
view(3)
axis('equal')
%axis vis3d