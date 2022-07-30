%close all
clear

dred = [0.6 0 0];
dgreen = [0 0.5 0];
dblue = [0 0 0.6];

% col1 = [89 148 184]/255; % blue
% col2 = [115 201 118]/255; % green
% col3 = [130 95 201]/255; % purple
% col4 = [201 148 75]/255; % orange
% col5 = [201 85 136]/255; % red

blue = [31/255 120/255 180/255];
green = [51/255 160/255 44/255];
purple = [106/255 61/255 154/255];
red = [227/255 26/255 28/255];
lgreen = [178/255 223/255 138/255];
lred = [251/255 154/255 153/255];
lblue = [166/255 206/255 227/255];
lpurple = [202/255 178/255 214/255];
% blue = col1;
% green = col2;
% red = col3;





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

%%
fig = figure('color',[1 1 1],'units','normalized','position', [.1,.1,.5,.6]);
%fig.Resize = 'off';
%set(fig,'defaultAxesColorOrder',[0 0 0; 0.6 0.2 0.1]);
axes('Linewidth',2,'FontSize',15);

hold on
set(gca, 'FontName', 'Arial')
axis equal
xlim([-125 175])
ylim([-100 450])
xlabel('x (mm)','FontSize',18);
ylabel('y (mm)','FontSize',18);

%xticks(-600:200:1000);
yticks(-100:100:400);

% gridlines
for m = 0:100:400
    plot([-117.5 175],[m m],'-','Color',[0.95 0.95 0.95],'LineWidth',1)
end
for m = -100:50:150
    plot([m m],[-92.5 450],'-','Color',[0.95 0.95 0.95],'LineWidth',1)
end

% puck position
puckR=76.2/2;
theta=-pi:0.01:pi;

PuckPos(:,:,1) = [MotionOne(1,1,1),MotionOne(1,2,1)];
PuckPos(:,:,2) = [MotionTwo(1,1,1),MotionTwo(1,2,1)];
PuckPos(:,:,3) = [MotionThr(1,1,1),MotionThr(1,2,1)];
PuckPos(:,:,4) = [MotionThr(size(MotionThr,1),1,1),MotionThr(size(MotionThr,1),2,1)];
% rectangle('Position', [PuckPos(1,1,1)-76.2/2 PuckPos(1,2,1)-76.2/2 76.2 76.2],'Curvature', [1 1],...
%     'FaceColor',[0 0 0 .1],'EdgeColor','none')
% rectangle('Position', [PuckPos(1,1,2)-76.2/2 PuckPos(1,2,2)-76.2/2 76.2 76.2],'Curvature', [1 1],...
%     'FaceColor',[31/255 120/255 180/255 .1],'EdgeColor','none')
% rectangle('Position', [PuckPos(1,1,3)-76.2/2 PuckPos(1,2,3)-76.2/2 76.2 76.2],'Curvature', [1 1],...
%     'FaceColor',[227/255 26/255 28/255 .1],'EdgeColor','none')
% rectangle('Position', [PuckPos(1,1,4)-76.2/2 PuckPos(1,2,4)-76.2/2 76.2 76.2],'Curvature', [1 1],...
%     'FaceColor',[51/255 160/255 44/255 .1],'EdgeColor','none')

xP1=puckR*cos(theta)+PuckPos(1,1,1);
yP1=puckR*sin(theta)+PuckPos(1,2,1);
fill3(xP1,yP1,zeros(1,numel(xP1)), green, 'FaceAlpha', 0.2, 'EdgeColor','none');
xP2=puckR*cos(theta)+PuckPos(1,1,2);
yP2=puckR*sin(theta)+PuckPos(1,2,2);
fill3(xP2,yP2,zeros(1,numel(xP2)), blue, 'FaceAlpha', 0.2, 'EdgeColor','none');
xP3=puckR*cos(theta)+PuckPos(1,1,3);
yP3=puckR*sin(theta)+PuckPos(1,2,3);
fill3(xP3,yP3,zeros(1,numel(xP3)), red, 'FaceAlpha', 0.2, 'EdgeColor','none');
xP4=puckR*cos(theta)+PuckPos(1,1,4);
yP4=puckR*sin(theta)+PuckPos(1,2,4);
fill3(xP4,yP4,zeros(1,numel(xP4)), [0 0 0], 'FaceAlpha', 0.1, 'EdgeColor','none');

% runs 1-3
runthickness = 2.5;
runmarker = 0.5;
run = 1;
plot(MotionOne(1,1,run),MotionOne(1,2,run),...
    '-o','Color',green+[0.15*(run-1) 0.1*(run-1) 0.15*(run-1)],'LineWidth',runthickness,'Markersize',runmarker)
plot(MotionTwo(1,1,run),MotionTwo(1,2,run),...
    '-o','Color',blue+[0.2*(run-1) 0.15*(run-1) 0.05*(run-1)],'LineWidth',runthickness,'Markersize',runmarker)
plot(MotionThr(1,1,run),MotionThr(1,2,run),...
    '-o','Color',red+[0 0.3*(run-1) 0.3*(run-1)],'LineWidth',runthickness,'Markersize',runmarker)
plot(MotionThr(size(MotionThr,1),1,run),MotionThr(size(MotionThr,1),2,run),...
    '-o','Color',[0 0 0 0.5],'LineWidth',runthickness,'Markersize',runmarker)


%circle around puck centre
scatter(PuckPos(1,1,1),PuckPos(1,2,1),50,'o','MarkerEdgeColor',green,'MarkerEdgeAlpha',0.7,'LineWidth',1)
scatter(PuckPos(1,1,2),PuckPos(1,2,2),50,'o','MarkerEdgeColor',blue,'MarkerEdgeAlpha',0.7,'LineWidth',1)
scatter(PuckPos(1,1,3),PuckPos(1,2,3),50,'o','MarkerEdgeColor',red,'MarkerEdgeAlpha',0.7,'LineWidth',1)
scatter(PuckPos(1,1,4),PuckPos(1,2,4),50,'o','MarkerEdgeColor',[0 0 0],'MarkerEdgeAlpha',0.3,'LineWidth',1)




%% 

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


%% Mori Arm modelling
totN = 3; % number of arms to show

armH = 550-502; % height from floor
numM = 4; % number of modules from anchor

% edge input (where next modules connects [eigher 1 or 2])
eCtrl = [2 1 2 1 2]; % must be numMod long
eCtrlCstm = 1; % for custom edge control, set edgeCtrlCstm = 1;

clrL = 28.55; % length between vertex and obstruction by module

% colors = jet(numM);
black = [0,0,0];
colors = [green; blue; red; black];

%% init
modV = zeros(totN,numM+1,3); % row=iter,col=mod,dep1=ang,ext,ext
modP = zeros(totN,numM+1,3,3); % rows=iter,col=mod,dep1=xyz,dep2=p1-3
basL = zeros(totN,1,1); % base edge length
basL(:,1) = 186;

modPV = zeros(totN,numM+1,3,12); % offset volume around module

% create chain if not custom
if eCtrlCstm == 0
    for m = 1:(numM)
        eCtrl(m) = rem(m,2) + 1;
    end
    clear m
end

% opposing corner of previous module
eOppo(1:numM) = 0;
for m = 1:(numM)
    if eCtrl(m) == 2
        eOppo(m) = 1;
    else
        eOppo(m) = 3;
    end
end
clear m


%% Generate starting configuration
% Cmd 4
modV(1,1,2:3) = 186;
modV(1,2,1) = deg2rad((1800-1633)/10);
modV(1,3,1) = deg2rad((1800-1642)/10);
modV(1,4,1) = deg2rad((1800-1990)/10);
modV(1,5,1) = deg2rad((1800-1943)/10);

modV(1,2,2) = 180+ 86 /10; % second module top (1)
modV(1,2,3) = 180+ 9 /10; % connected (3)

modV(1,3,2) = 180+ 53 /10; % third module bottom (2)
modV(1,3,3) = 180+ 114 /10; % connected (3)

modV(1,4,2) = 180+ 30 /10; % fourth module connected (3)
modV(1,4,3) = 180+ 84 /10; % top (1)

modV(1,5,2) = 180+ 65 /10; % fifth module bottom (3)
modV(1,5,3) = 180+ 49 /10; % (1)

% Cmd 13
modV(2,1,2:3) = 186;
modV(2,2,1) = deg2rad((1800-1894)/10);
modV(2,3,1) = deg2rad((1800-2073)/10);
modV(2,4,1) = deg2rad((1800-1687)/10);
modV(2,5,1) = deg2rad((1800-1542)/10);

modV(2,2,2) = 180+ 120 /10; % second module top (1)
modV(2,2,3) = 180+ 3 /10; % connected (3)

modV(2,3,2) = 180+ 88 /10; % third module bottom (2)
modV(2,3,3) = 180+ 120 /10; % connected (3)

modV(2,4,2) = 180+ 36 /10; % fourth module connected (3)
modV(2,4,3) = 180+ 120 /10; % top (1)

modV(2,5,2) = 180+ 77 /10; % fifth module bottom (3)
modV(2,5,3) = 180+ 120 /10; % (1)

% Cmd 23
modV(3,1,2:3) = 186;
modV(3,2,1) = deg2rad((1800-2283)/10);
modV(3,3,1) = deg2rad((1800-1666)/10);
modV(3,4,1) = deg2rad((1800-866)/10);
modV(3,5,1) = deg2rad((1800-1287)/10);

modV(3,2,2) = 180+ 3 /10; % second module top (1)
modV(3,2,3) = 180+ 3 /10; % connected (3)

modV(3,3,2) = 180+ 106 /10; % third module bottom (2)
modV(3,3,3) = 180+ 120 /10; % connected (3)

modV(3,4,2) = 180+ 10 /10; % fourth module connected (3)
modV(3,4,3) = 180+ 3 /10; % top (1)

modV(3,5,2) = 180+ 3 /10; % fifth module bottom (3)
modV(3,5,3) = 180+ 120 /10; % (1)

for n = 1:1:totN
modV(n,1,2:3) = basL(1);
modP(n,1,:,:) = [0,0,0;...
    -basL(1)/2, basL(1)/2, 0;...
    armH, armH, armH+sqrt(basL(1)^2-(basL(1)/2)^2)];
modPV(n,1,:,1:6) = [6,6,6,6,6,6;... %y
    -basL(1)+clrL+basL(1)/2,-clrL+basL(1)/2,... %x1
    -clrL*cos(pi/3)+basL(1)/2,-basL(1)/2+clrL*cos(pi/3)+basL(1)/2,... %x2
    -basL(1)/2-clrL*cos(pi/3)+basL(1)/2,-basL(1)+clrL*cos(pi/3)+basL(1)/2;... %x3
    armH,armH,... %z1
    armH+clrL*sin(pi/3),armH+sqrt(basL(1)^2-(basL(1)/2)^2)-clrL*sin(pi/3),... %z2
    armH+sqrt(basL(1)^2-(basL(1)/2)^2)-clrL*sin(pi/3),armH+clrL*sin(pi/3)]; %z3
modPV(n,1,:,7:12) = modPV(n,1,:,1:6);
modPV(n,1,1,7:12) = -modPV(n,1,1,7:12);

for i = 2:(numM+1)
    % circle intersection to find next module vertex
    % https://math.stackexchange.com/questions/2313654/circle-circle-intersection-cordinates-3d-normal-plane
    v = (modP(n,i-1,:,eCtrl(i-1)+1) - modP(n,i-1,:,eCtrl(i-1))) / ...
        sqrt((modP(n,i-1,1,eCtrl(i-1)+1) - modP(n,i-1,1,eCtrl(i-1)))^2 +...
        (modP(n,i-1,2,eCtrl(i-1)+1) - modP(n,i-1,2,eCtrl(i-1)))^2 +...
        (modP(n,i-1,3,eCtrl(i-1)+1) - modP(n,i-1,3,eCtrl(i-1)))^2);
    nv = cross(modP(n,i-1,:,eCtrl(i-1)+1) - modP(n,i-1,:,eCtrl(i-1)),...
        modP(n,i-1,:,eCtrl(i-1)+1) - modP(n,i-1,:,eOppo(i-1)));
    w = cross(nv/vecnorm(nv),v);
    
    % angle and intersection lengths
    aP2 = acos((modV(n,i-1,eCtrl(i-1)+1)^2 + modV(n,i,2)^2 -modV(n,i,3)^2)/(2*modV(n,i-1,eCtrl(i-1)+1)*modV(n,i,2)));
    h = modV(n,i,2)*sin(aP2);
    a = modV(n,i,2)*cos(aP2);
    
    % intersection points
    P1 = modP(n,i-1,:,eCtrl(i-1)) + a*v + h*w;
    P2 = modP(n,i-1,:,eCtrl(i-1)) + a*v - h*w;
    
    % select farther intersection point
    if sqrt(sum((P1-modP(n,i-1,:,eOppo(i-1))).^2)) ...
            <= sqrt(sum((P2-modP(n,i-1,:,eOppo(i-1))).^2))
        P3 = P2;
    else
        P3 = P1;
    end
    
    % next module coordinates
    modP(n,i,:,:) = [modP(n,i-1,1,eCtrl(i-1)), P3(1), modP(n,i-1,1,eCtrl(i-1)+1);...
        modP(n,i-1,2,eCtrl(i-1)), P3(2), modP(n,i-1,2,eCtrl(i-1)+1);...
        modP(n,i-1,3,eCtrl(i-1)), P3(3), modP(n,i-1,3,eCtrl(i-1)+1)];
    
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % rotate new point about axis of previous points
    % http://paulbourke.net/geometry/rotate/
    % step 1: translate so that coupling axis passes through origin
    q1 = modP(n,i,:,2) - modP(n,i,:,1);
    u1 = modP(n,i,:,3) - modP(n,i,:,1);
    u = u1 / vecnorm(u1);
    d = sqrt(u(2)^2 + u(3)^2);
    
    % step 2: rotate about x axis so that coupling axis is in xz plane
    q2(1:3) = 0;
    if d ~= 0
        q2(1) = q1(1);
        q2(2) = q1(2)*u(3)/d - q1(3)*u(2)/d;
        q2(3) = q1(2)*u(2)/d + q1(3)*u(3)/d;
    else
        q2 = q1;
    end
    
    % step 3: rotate about y axis so that coupling axis lies on z axis
    q1(1) = q2(1)*d - q2(3)*u(1);
    q1(2) = q2(2);
    q1(3) = q2(1)*u(1) + q2(3)*d;
    
    % step 4: perform the desired rotation by theta about the z axis
    q2(1) = q1(1)*cos(modV(n,i,1)) - q1(2)*sin(modV(n,i,1));
    q2(2) = q1(1)*sin(modV(n,i,1)) + q1(2)*cos(modV(n,i,1));
    q2(3) = q1(3);
    
    % step 5: apply the inverse of step 3
    q1(1) = q2(1)*d + q2(3)*u(1);
    q1(2) = q2(2);
    q1(3) = - q2(1)*u(1) + q2(3)*d;
    
    % step 6: apply the inverse of step 2
    if d ~= 0
        q2(1) = q1(1);
        q2(2) = q1(2)*u(3)/d + q1(3)*u(2)/d;
        q2(3) = - q1(2)*u(2)/d + q1(3)*u(3)/d;
    else
        q2 = q1;
    end
    
    % step 7: apply the inverse of step 1
    q1(:) = q2(:) + squeeze(modP(n,i,:,1));
    modP(n,i,:,2) = q1; % update final module coordinate after rotation
    
    % generate volume
    iter = 0;
    for vert = 1:3
        for vert2 = 1:3
            if vert == vert2
                continue
            end
            % create 6 points on edges 28.55mm from vertices
            iter = iter+1;
            nNew = modP(n,i,:,vert)-modP(n,i,:,vert2);
            wNew = nNew/vecnorm(nNew);
            modPV(n,i,:,iter) = modP(n,i,:,vert) - 28.55*wNew;
            
            % move 6 points outwards by 6mm orthogonal to edge
            nNew = mean(modP(n,i,:,[vert vert2]),4) ...
                -mean(modP(n,i,:,:),4);
            wNew = nNew/vecnorm(nNew);
            modPV(n,i,:,iter) = modPV(n,i,:,iter) + 6*wNew;
        end
    end
    % offset points from plane by 6 in both directions
    nNew = cross(modP(n,i,:,2)-modP(n,i,:,1),...
        modP(n,i,:,3)-modP(n,i,:,1));
    wNew = nNew/vecnorm(nNew);
    for vert = 1:6
        modPV(n,i,:,vert) = modPV(n,i,:,vert) + 6*wNew;
    end
    for vert = 7:12
        modPV(n,i,:,vert) = modPV(n,i,:,vert-6) - 12*wNew;
    end
end
end


%% Plot
%plot moving configurations (start)
pltD = squeeze(modP(1,1,:,:));
ax=gca; ax.SortMethod='childorder'; %avoid transparency issue
plot3([pltD(1,1) pltD(1,2) pltD(1,3) pltD(1,1)],...
    [pltD(2,1) pltD(2,2) pltD(2,3) pltD(2,1)],...
    [pltD(3,1) pltD(3,2) pltD(3,3) pltD(3,1)],...
    '-.','Color',[0.2 0.2 0.2 1],'LineWidth',2);

q = 1;
pltDV = squeeze(modPV(q,1,:,:));%volume
tri = convhull(pltDV(1,:),pltDV(2,:),pltDV(3,:),'Simplify',true);
pltDVP = trimesh(tri,pltDV(1,:),pltDV(2,:),pltDV(3,:),...
        'facecolor',[0.2 0.2 0.2],'facealpha',.5,'edgecolor','none');
for q = 1:1:totN
    for nn = 2:(numM+1)%rest
        pltM(nn-1) = plot3(squeeze(modP(q,nn,1,:)),...
            squeeze(modP(q,nn,2,:)),...
            squeeze(modP(q,nn,3,:)),...
            '-.','Color',[colors(q,:) 0.6],'LineWidth',1.5);
        pltDV = squeeze(modPV(q,nn,:,:));
        tri = convhull(pltDV(1,:),pltDV(2,:),pltDV(3,:),'Simplify',true);
        pltV(q,nn-1) = trimesh(tri,pltDV(1,:),pltDV(2,:),pltDV(3,:),...
            'facecolor',colors(q,:),'facealpha',.25,'edgecolor','none');
    end
end

%%

title('Manipulation experiment — configurations')
subtitle(' ')
% title(['This is an example for emdash' '---'],'Interpreter','Latex')
% legend([L1 L2 L3 L4], {'Input position', 'Run 1', 'Run 2', 'Run 3'...
%     'Internal edge model'},'FontSize',17,...
%     'LineWidth',1.5,'NumColumns',2)

[lh, labelhandles] = legend([pltDVP,pltV(1,1),pltV(2,1),pltV(3,1)],{'Base module','Right push config.','Left push config.','Front push config.'},...
    'FontSize',13, 'LineWidth',1,'NumColumns',2,'Location','southeast');


fh = findall(0,'Type','Figure');
txt_obj = findall(fh,'Type','text');
set(txt_obj,'FontName','Arial');


print(fig, '-painters', 'Arm_Overhead_01.eps', '-depsc')

printeps(1,'Arm_Overhead_01test');










