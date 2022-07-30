%close all
clear

blue = [31/255 120/255 180/255];
green = [51/255 160/255 44/255];
purple = [106/255 61/255 154/255];
red = [227/255 26/255 28/255];


%%
YY = 75;
linegap = 0.3;
Input(:,:,1) = [
-100+6+76.2/2, YY-linegap
76.2/2+6, YY-linegap
];

Input(:,:,2) = [
76.2/2+6, YY+linegap
25-6-76.6/2, YY+linegap
];

Input(:,:,3) = [
25-6-76.6/2, YY+linegap
25-6-76.6/2, YY+35
];

%%
BoundingBox(:,:,1) = [
-305.05	103.37
288.7	103.37
292.01	-500.31
-308.36	-495.35
-305.05	103.37
];

BoundingBox(:,:,2) = [
-304.33	103.35
291.1	101.98
295.21	-501.67
-307.07	-494.83
-304.33	103.35
];

BoundingBox(:,:,3) = [
-302.97	106.08
293.84	107.45
297.95	-494.83
-304.33	-490.72
-302.97	106.08
];


ScaleFactor = zeros(1,3);
for run = 1:1:3
    Origin(1,1,run) = mean(BoundingBox(1:4,1,run));
    Origin(1,2,run) = mean(BoundingBox(1:4,2,run));
    for i = 1:1:4
        ScaleFactor(run) = ScaleFactor(run) + norm(BoundingBox(i+1,:,run)-BoundingBox(i,:,run));
    end
ScaleFactor(run) = 609.6/(ScaleFactor(run)/4); % 609.6 is 24 inches (sizeo of mat)
end

%%
ExtendedArm(:,:,1) = [
-31.94	-485.25
-30.57	-55.44
];

ExtendedArm(:,:,2) = [
-31.94	-486.62
-29.2	-56.81
];

ExtendedArm(:,:,3) = [
-29.2	-483.88
-29.2	-51.33
];

for j = 1:3
    ExtendedArm(:,1,j) = (ExtendedArm(:,1,j)-Origin(1,1,j))*ScaleFactor(j);
    ExtendedArm(:,2,j) = (ExtendedArm(:,2,j)-Origin(1,2,j))*ScaleFactor(j);
end

GlobalXOffset = 25-6-76.6/2     +8;
GlobalYOffset = (ExtendedArm(1,2,1)+ExtendedArm(1,2,2)+ExtendedArm(1,2,3))/3;
BaseXOffset = 0;
for j = 1:3
    BaseXOffset = BaseXOffset + ExtendedArm(1,1,j) + ExtendedArm(2,1,j);
end
BaseXOffset = BaseXOffset/12 + GlobalXOffset;
BaseYOffset = 65 + GlobalYOffset;

%% 
MotionOne(:,:,1) = [
-75.16	-123.22
-63.42	-123.88
-49.73	-121.14
-34.68	-119.77
-20.99	-117.03
-8.67	-117.03
9.13	-117.03
25.55	-117.03
];

MotionOne(:,:,2) = [
-68.9	-125.25
-64.79	-123.88
-51.1	-122.51
-33.31	-117.03
-20.99	-115.67
-4.56	-115.67
9.13	-112.93
26.92	-114.3
];

MotionOne(:,:,3) = [
-71.63	-118.4
-63.42	-118.4
-49.73	-115.67
-33.31	-112.93
-19.62	-112.93
-7.3	-110.19
9.13	-110.19
28.29	-108.82
];

for j = 1:3
    MotionOne(:,1,j) = (MotionOne(:,1,j)-Origin(1,1,j))*ScaleFactor(j) - BaseXOffset;
    MotionOne(:,2,j) = (MotionOne(:,2,j)-Origin(1,2,j))*ScaleFactor(j) - BaseYOffset;
end

%%
MotionTwo(:,:,1) = [
25.55	-117.03
15.97	-114.3
0.91	-112.93
-11.41	-112.93
-26.46	-111.56
-41.52	-111.56
];

MotionTwo(:,:,2) = [
26.92	-114.3
18.71	-114.3
13.23	-114.3
0.91	-114.3
-12.78	-114.3
-40.15	-110.19
];

MotionTwo(:,:,3) = [
28.29	-108.82
15.97	-108.82
0.91	-107.45
-23.73	-106.08
-26.46	-107.45
-40.15	-104.71
];

for j = 1:3
    MotionTwo(:,1,j) = (MotionTwo(:,1,j)-Origin(1,1,j))*ScaleFactor(j) - BaseXOffset;
    MotionTwo(:,2,j) = (MotionTwo(:,2,j)-Origin(1,2,j))*ScaleFactor(j) - BaseYOffset;
end

%%
MotionThr(:,:,1) = [
-41.52	-111.56
-44.26	-107.45
-40.15	-96.5
-38.78	-92.4
];

MotionThr(:,:,2) = [
-40.15	-110.19
-40.15	-107.45
-40.15	-97.87
-38.78	-91.03
];

MotionThr(:,:,3) = [
-40.15	-104.71
-41.28	-101.72
-41.52	-92.4
-38.78	-85.55
];

for j = 1:3
    MotionThr(:,1,j) = (MotionThr(:,1,j)-Origin(1,1,j))*ScaleFactor(j) - BaseXOffset;
    MotionThr(:,2,j) = (MotionThr(:,2,j)-Origin(1,2,j))*ScaleFactor(j) - BaseYOffset;
end

%% PLOT

fig = figure('color',[1 1 1],'units','normalized','position', [.1,.1,.5,.6]);
%fig.Resize = 'off';
%set(fig,'defaultAxesColorOrder',[0 0 0; 0.6 0.2 0.1]);
axes('Linewidth',2,'FontSize',14);

hold on
set(gca, 'FontName', 'Arial')
axis equal
xlim([-90 100])
ylim([260 380])
xlabel('x (mm)','FontSize',18);
ylabel('y (mm)','FontSize',18);

% gridlines
for m = 280:20:380
    plot([-87.5 100],[m m],'-','Color',[0.95 0.95 0.95],'LineWidth',1)
end
for m = -80:20:100
    plot([m m],[262.5 380],'-','Color',[0.95 0.95 0.95],'LineWidth',1)
end

% puck position
PuckPos(:,:,1) = [MotionOne(1,1,1),MotionOne(1,2,1)];
PuckPos(:,:,2) = [MotionTwo(1,1,1),MotionTwo(1,2,1)];
PuckPos(:,:,3) = [MotionThr(1,1,1),MotionThr(1,2,1)];
PuckPos(:,:,4) = [MotionThr(size(MotionThr,1),1,1),MotionThr(size(MotionThr,1),2,1)];
rectangle('Position', [PuckPos(1,1,1)-76.2/2 PuckPos(1,2,1)-76.2/2 76.2 76.2],'Curvature', [1 1],...
    'FaceColor',[51/255 160/255 44/255 .1],'EdgeColor','none')
rectangle('Position', [PuckPos(1,1,2)-76.2/2 PuckPos(1,2,2)-76.2/2 76.2 76.2],'Curvature', [1 1],...
    'FaceColor',[31/255 120/255 180/255 .1],'EdgeColor','none')
rectangle('Position', [PuckPos(1,1,3)-76.2/2 PuckPos(1,2,3)-76.2/2 76.2 76.2],'Curvature', [1 1],...
    'FaceColor',[227/255 26/255 28/255 .1],'EdgeColor','none')
rectangle('Position', [PuckPos(1,1,4)-76.2/2 PuckPos(1,2,4)-76.2/2 76.2 76.2],'Curvature', [1 1],...
    'FaceColor',[0 0 0 .1],'EdgeColor','none')

% input
inputmarker = 16;
arrowmarker = 9;

plot(Input(:,1,3) - GlobalXOffset,Input(:,2,3) - BaseYOffset,...
    '--x','Color',red,'LineWidth',2,'Markersize',inputmarker,'MarkerFaceColor',red)
plot((Input(1,1,3)+Input(2,1,3))/2 - GlobalXOffset,(Input(1,2,3)+Input(2,2,3))/2 - BaseYOffset,'^',...
    'Color',red,'LineWidth',2,'Markersize',arrowmarker,'MarkerFaceColor',red)

plot(Input(:,1,2) - GlobalXOffset,Input(:,2,2) - BaseYOffset,...
    '--x','Color',blue,'LineWidth',2,'Markersize',inputmarker,'MarkerFaceColor',blue)
plot((Input(1,1,2)+Input(2,1,2))/2 - GlobalXOffset,(Input(1,2,2)+Input(2,2,2))/2 - BaseYOffset,'<',...
    'Color',blue,'LineWidth',2,'Markersize',arrowmarker,'MarkerFaceColor',blue)

L1 = plot(Input(:,1,1) - GlobalXOffset,Input(:,2,1) - BaseYOffset,...
    '--x','Color',green,'LineWidth',2,'Markersize',inputmarker,'MarkerFaceColor',green);
plot((Input(1,1,1)+Input(2,1,1))/2 - GlobalXOffset,(Input(1,2,1)+Input(2,2,1))/2 - BaseYOffset,'>',...
    'Color',green,'LineWidth',2,'Markersize',arrowmarker,'MarkerFaceColor',green)

% runs 1-3
runthickness = 2.5;
runmarker = 4;
for run = 3:-1:1
    plot(MotionOne(:,1,run),MotionOne(:,2,run),...
        '-','Color',green+[0.25*(run-1) 0.1*(run-1) 0.25*(run-1)],'LineWidth',...
        runthickness,'Markersize',runmarker,'MarkerFaceColor',green+[0.25*(run-1) 0.1*(run-1) 0.25*(run-1)])
    plot(MotionTwo(:,1,run),MotionTwo(:,2,run),...
        '-','Color',blue+[0.25*(run-1) 0.15*(run-1) 0.05*(run-1)],'LineWidth',...
        runthickness,'Markersize',runmarker,'MarkerFaceColor',blue+[0.25*(run-1) 0.15*(run-1) 0.05*(run-1)])
    plot(MotionThr(:,1,run),MotionThr(:,2,run),...
        '-','Color',red+[0 0.25*(run-1) 0.25*(run-1)],'LineWidth',...
        runthickness,'Markersize',runmarker,'MarkerFaceColor',red+[0 0.25*(run-1) 0.25*(run-1)])
end

%circle around puck centre
circleline = 4;
circlemarker = 250;
scatter(PuckPos(1,1,1),PuckPos(1,2,1),circlemarker,'o','MarkerEdgeColor',green,...
    'MarkerEdgeAlpha',0.5,'LineWidth',circleline)
scatter(PuckPos(1,1,2),PuckPos(1,2,2),circlemarker,'o','MarkerEdgeColor',blue,...
    'MarkerEdgeAlpha',0.5,'LineWidth',circleline)
scatter(PuckPos(1,1,3),PuckPos(1,2,3),circlemarker,'o','MarkerEdgeColor',red,...
    'MarkerEdgeAlpha',0.5 ,'LineWidth',circleline)
scatter(PuckPos(1,1,4),PuckPos(1,2,4),circlemarker,'o','MarkerEdgeColor',[0 0 0],...
    'MarkerEdgeAlpha',0.3,'LineWidth',circleline)


%%
%cosmetic lines for legend
L2 = plot([-1000 -1000],[-1000 -1000],...
    '-o','Color',blue,'LineWidth',runthickness,'Markersize',runmarker);

L3 = scatter([-0 -0],[-0 -0],5000);
set(L3, 'MarkerEdgeColor','none','MarkerEdgeAlpha',0,'MarkerFaceColor',[0.8 0.8 0.8],'MarkerFaceAlpha',0.01);
%...
%    'MarkerEdgeColor','none','MarkerFaceColor',[0 0 0],'MarkerFaceAlpha',0.1);

title('Manipulation experiment — object position data','FontSize',18)
subtitle(' ')
% title(['This is an example for emdash' '---'],'Interpreter','Latex')
% legend([L1 L2 L3 L4], {'Input position', 'Run 1', 'Run 2', 'Run 3'...
%     'Internal edge model'},'FontSize',17,...
%     'LineWidth',1.5,'NumColumns',2)

[lh, labelhandles] = legend([L1 L2 L3], {'Input trajectories','Output position','Key puck outlines'},...
    'FontSize',16, 'LineWidth',1,'NumColumns',2);
% labelhandles(5).LineStyle = '--';
% labelhandles(4).YData = [0.83 0.83];
% labelhandles(5).XData = [0.0460 0.5057];
% labelhandles(5).YData = [0.760 0.760];

objhl = findobj(labelhandles,'Type','patch');
set(objhl, 'MarkerSize', 18)

%print(fig, '-painters', 'ArmDataPlot_01.eps', '-depsc')



fh = findall(0,'Type','Figure');
txt_obj = findall(fh,'Type','text');
set(txt_obj,'FontName','Arial');


print(fig, '-painters', 'Arm_Analysis_01.eps', '-depsc')

printeps(1,'Arm_Analysis_01test');
