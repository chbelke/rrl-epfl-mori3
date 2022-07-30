close all
clear
Track_Data; % load data
Track_Model; % load model

blue = [31/255 120/255 180/255];
green = [51/255 160/255 44/255];
purple = [106/255 61/255 154/255];
red = [227/255 26/255 28/255];

%%
% centre of module
centre_straight(:,1,:) = (track_straight(:,1,:) + track_straight(:,3,:) + track_straight(:,5,:))/3;
centre_straight(:,2,:) = (track_straight(:,2,:) + track_straight(:,4,:) + track_straight(:,6,:))/3;
centre_halfleft(:,1,:) = (track_halfleft(:,1,:) + track_halfleft(:,3,:) + track_halfleft(:,5,:))/3;
centre_halfleft(:,2,:) = (track_halfleft(:,2,:) + track_halfleft(:,4,:) + track_halfleft(:,6,:))/3;
centre_fullleft(:,1,:) = (track_fullleft(:,1,:) + track_fullleft(:,3,:) + track_fullleft(:,5,:))/3;
centre_fullleft(:,2,:) = (track_fullleft(:,2,:) + track_fullleft(:,4,:) + track_fullleft(:,6,:))/3;

% shift to origin
centre_straight(:,:,:) = centre_straight(:,:,:) - centre_straight(1,:,:);
centre_halfleft(:,:,:) = centre_halfleft(:,:,:) - centre_halfleft(1,:,:);
centre_fullleft(:,:,:) = centre_fullleft(:,:,:) - centre_fullleft(1,:,:);

% rotate by initline
theta_straight(1:3) = 0;
theta_halfleft(1:3) = 0;
theta_fullleft(1:3) = 0;
R_straight = zeros(2,2,3);
R_halfleft = zeros(2,2,3);
R_fullleft = zeros(2,2,3);
centre_straight_rotated = zeros(size(centre_straight));
centre_halfleft_rotated = zeros(size(centre_halfleft));
centre_fullleft_rotated = zeros(size(centre_fullleft));
for p  = 1:1:3
    theta_straight(p) = -sin(...
        (initline_straight(:,2,p) - initline_straight(:,4,p))...
        / (initline_straight(:,1,p) - initline_straight(:,3,p)));
    R_straight(:,:,p) = [cos(theta_straight(p)) -sin(theta_straight(p));...
        sin(theta_straight(p)) cos(theta_straight(p))];
    centre_straight_rotated(:,:,p) = (R_straight(:,:,p)*centre_straight(:,:,p)')';
    
    theta_halfleft(p) = -sin(...
        (initline_halfleft(:,2,p) - initline_halfleft(:,4,p))...
        / (initline_halfleft(:,1,p) - initline_halfleft(:,3,p)));
    R_halfleft(:,:,p) = [cos(theta_halfleft(p)) -sin(theta_halfleft(p));...
        sin(theta_halfleft(p)) cos(theta_halfleft(p))];
    centre_halfleft_rotated(:,:,p) = (R_halfleft(:,:,p)*centre_halfleft(:,:,p)')';
    
    theta_fullleft(p) = -sin(...
        (initline_fullleft(:,2,p) - initline_fullleft(:,4,p))...
        / (initline_fullleft(:,1,p) - initline_fullleft(:,3,p)));
    R_fullleft(:,:,p) = [cos(theta_fullleft(p)) -sin(theta_fullleft(p));...
        sin(theta_fullleft(p)) cos(theta_fullleft(p))];
    centre_fullleft_rotated(:,:,p) = (R_fullleft(:,:,p)*centre_fullleft(:,:,p)')';
end

ScaleFactor = 609.6/((884.71+856.43)/3); % 609.6 is 24 inches (sizeo of mat)
ScaleFactor = 1.158524 / ScaleFactor; % from drive doc
centre_straight_rotated(:,:,:) = centre_straight_rotated(:,:,:)/ScaleFactor;
centre_halfleft_rotated(:,:,:) = centre_halfleft_rotated(:,:,:)/ScaleFactor;
centre_fullleft_rotated(:,:,:) = centre_fullleft_rotated(:,:,:)/ScaleFactor;

% secondary line shifted up to starting point
secondary_offset_straight = [mean(centre_straight_rotated(2,1,:)) - model_straight(2,1),...
    mean(centre_straight_rotated(2,2,:)) - model_straight(2,2)];
secondary_offset_halfleft = [mean(centre_halfleft_rotated(2,1,:)) - model_halfleft(2,1),...
    mean(centre_halfleft_rotated(2,2,:)) - model_halfleft(2,2)];
secondary_offset_fullleft = [mean(centre_fullleft_rotated(2,1,:)) - model_fullleft(2,1),...
    mean(centre_fullleft_rotated(2,2,:)) - model_fullleft(2,2)];

for s = 2:2:14
    centre_straight_rotated(s,:,:) = centre_straight_rotated(s,:,:) - secondary_offset_straight;
    centre_halfleft_rotated(s,:,:) = centre_halfleft_rotated(s,:,:) - secondary_offset_halfleft;
    centre_fullleft_rotated(s,:,:) = centre_fullleft_rotated(s,:,:) - secondary_offset_fullleft;
end

% shift left going ones up for plot
plotshift_halfleft = 250;
plotshift_fullleft = 600;

centre_halfleft_rotated(:,2,:) = centre_halfleft_rotated(:,2,:) + plotshift_halfleft;
centre_fullleft_rotated(:,2,:) = centre_fullleft_rotated(:,2,:) + plotshift_fullleft;


%% PLOT
fig = figure('color',[1 1 1],'units','normalized','position', [.1,.1,.5,.65]);
axes('Linewidth',2,'FontSize',13);

hold on
set(gca, 'FontName', 'Arial')
axis equal
xlim([-130 1300])
ylim([-150 950])
xlabel('x (mm)','FontSize',16);
ylabel('y (mm)','FontSize',16);
xticks(-100:100:1200)
yticks([-100 -50 0 50  150 200 250 300 350 400  500 550 600 650 700 750 800 850 900 ])

title('Track locomotion data','FontSize',16)


% gridlines
for m = -100:50:50
    plot([-110 1280],[m m],'-','Color',[0.95 0.95 0.95],'LineWidth',1)
end
for m = 150:50:400
    plot([-110 1280],[m m],'-','Color',[0.95 0.95 0.95],'LineWidth',1)
end
for m = 500:50:900
    plot([-110 1280],[m m],'-','Color',[0.95 0.95 0.95],'LineWidth',1)
end

for m = -100:100:1200
    plot([m m],[-130 70],'-','Color',[0.95 0.95 0.95],'LineWidth',1)
end
for m = -100:100:1200
    plot([m m],[130 420],'-','Color',[0.95 0.95 0.95],'LineWidth',1)
end
for m = -100:100:1200
    plot([m m],[480 920],'-','Color',[0.95 0.95 0.95],'LineWidth',1)
end


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


%% Generate triangles
totN = 3; % number of arms to show
armH = -51.961; % height from floor
numM = 13; % number of modules from anchor
clrL = 28.55; % length between vertex and obstruction by module

% colors = jet(numM);
black = [0,0,0];
colors = [green; blue; red; black];

% init
modV = zeros(totN,numM+1,3); % row=iter,col=mod,dep1=ang,ext,ext
modP = zeros(totN,numM+1,3,3); % rows=iter,col=mod,dep1=xyz,dep2=p1-3
basL = zeros(totN,1,1); % base edge length
basL(:,1) = 180;

modPV = zeros(totN,numM+1,3,12); % offset volume around module

% create chain if not custom
for m = 1:(numM)
    eCtrl(m) = rem(m,2) + 1;
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

% straight
modV(1,1:(numM+1),1) = deg2rad(1800);
modV(1,[2 4 6 8 10 12 14],2) = 180;
modV(1,[2 4 6 8 10 12 14],3) = 180;
modV(1,[1 3 5 7 9 11 13],2) = 180;
modV(1,[1 3 5 7 9 11 13],3) = 180;

% halfleft
modV(2,1:(numM+1),1) = deg2rad(1800);
modV(2,[2 4 6 8 10 12 14],2) = 180;
modV(2,[2 4 6 8 10 12 14],3) = 186;
modV(2,[1 3 5 7 9 11 13],2) = 180;
modV(2,[1 3 5 7 9 11 13],3) = 180;

% fullleft
modV(3,1:(numM+1),1) = deg2rad(1800);
modV(3,[2 4 6 8 10 12 14],2) = 180;
modV(3,[2 4 6 8 10 12 14],3) = 192;
modV(3,[1 3 5 7 9 11 13],2) = 180;
modV(3,[1 3 5 7 9 11 13],3) = 180;

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


startmodulealpha = 1;
startmodulecolor = [.25 .25 .25];
startmodulewidth = 1;
pltD = squeeze(modP(1,1,:,:));
ax=gca; ax.SortMethod='childorder'; %avoid transparency issue
L2 = plot3([pltD(2,1) pltD(2,2) pltD(2,3) pltD(2,1)],...
    -[pltD(3,1) pltD(3,2) pltD(3,3) pltD(3,1)],...
    [pltD(1,1) pltD(1,2) pltD(1,3) pltD(1,1)],...
    '-.','Color',[startmodulecolor startmodulealpha],'LineWidth',startmodulewidth);
plot3([pltD(2,1) pltD(2,2) pltD(2,3) pltD(2,1)],...
    -[pltD(3,1) pltD(3,2) pltD(3,3) pltD(3,1)] + plotshift_halfleft,...
    [pltD(1,1) pltD(1,2) pltD(1,3) pltD(1,1)],...
    '-.','Color',[startmodulecolor startmodulealpha],'LineWidth',startmodulewidth);
plot3([pltD(2,1) pltD(2,2) pltD(2,3) pltD(2,1)],...
    -[pltD(3,1) pltD(3,2) pltD(3,3) pltD(3,1)] + plotshift_fullleft,...
    [pltD(1,1) pltD(1,2) pltD(1,3) pltD(1,1)],...
    '-.','Color',[startmodulecolor startmodulealpha],'LineWidth',startmodulewidth);

modP(2,:,3,:) = modP(2,:,3,:) - plotshift_halfleft;
modP(3,:,3,:) = modP(3,:,3,:) - plotshift_fullleft;

q = 1;
%pltDV = squeeze(modPV(q,1,:,:));%volume
%tri = convhull(pltDV(2,:),-pltDV(3,:),pltDV(1,:),'Simplify',true);
%pltDVP = trimesh(tri,pltDV(2,:),-pltDV(3,:),pltDV(1,:),...
%        'facecolor',[0.2 0.2 0.2],'facealpha',.5,'edgecolor','none');
run = 2;
colors2 = [
    green-[0.15*(run-1) 0.35*(run-1) 0.15*(run-1)]
    blue-[0.1*(run-1) 0.4*(run-1) 0.45*(run-1)]
    red-[0.4 0.1*(run-1) 0.1*(run-1)]
];

for q = 1:1:totN
    for nn = 2:(numM+1)%rest
        pltM(nn-1) = plot3(squeeze(modP(q,nn,2,:)),...
            -squeeze(modP(q,nn,3,:)),...
            squeeze(modP(q,nn,1,:)),...
            '-.','Color',[colors(q,:) 0.75],'LineWidth',1);
            %'-.','Color',[0 0 0 0.1],'LineWidth',1.5);
        %pltDV = squeeze(modPV(q,nn,:,:));
        %tri = convhull(pltDV(2,:),-pltDV(3,:),pltDV(1,:),'Simplify',true);
        %pltV(q,nn-1) = trimesh(tri,pltDV(2,:),-pltDV(3,:),pltDV(1,:),...
        %    'facecolor',colors(q,:),'facealpha',.25,'edgecolor','none');
    end
end


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


%% Plot data

% input
run = 2;
targetthickness = 1;
targetmarker = 8;
plot(model_straight(:,1),model_straight(:,2),...
    'x','Color',green-[0.15*(run-1) 0.35*(run-1) 0.15*(run-1)],'LineWidth',targetthickness,'Markersize',targetmarker,'MarkerFaceColor','none');
plot(model_halfleft(:,1),model_halfleft(:,2) + plotshift_halfleft,...
    'x','Color',blue-[0.1*(run-1) 0.4*(run-1) 0.45*(run-1)],'LineWidth',targetthickness,'Markersize',targetmarker,'MarkerFaceColor','none')
plot(model_fullleft(:,1),model_fullleft(:,2) + plotshift_fullleft,...
    'x','Color',red-[0.4 0.1*(run-1) 0.1*(run-1)],'LineWidth',targetthickness,'Markersize',targetmarker,'MarkerFaceColor','none')

% targetlinealpha = 0.2;
% plot(model_straight([1 3 5 7 9 11 13],1),model_straight([1 3 5 7 9 11 13],2),...
%     '-','Color',[green-[0.15*(run-1) 0.35*(run-1) 0.15*(run-1)] targetlinealpha],'LineWidth',targetthickness,'Markersize',targetmarker,'MarkerFaceColor','none');
% plot(model_halfleft([1 3 5 7 9 11 13],1),model_halfleft([1 3 5 7 9 11 13],2) + plotshift_halfleft,...
%     '-','Color',[blue-[0.1*(run-1) 0.4*(run-1) 0.45*(run-1)] targetlinealpha],'LineWidth',targetthickness,'Markersize',targetmarker,'MarkerFaceColor','none')
% plot(model_fullleft([1 3 5 7 9 11 13],1),model_fullleft([1 3 5 7 9 11 13],2) + plotshift_fullleft,...
%     '-','Color',[red-[0.4 0.1*(run-1) 0.1*(run-1)] targetlinealpha],'LineWidth',targetthickness,'Markersize',targetmarker,'MarkerFaceColor','none')
% plot(model_straight([2 4 6 8 10 12 14],1),model_straight([2 4 6 8 10 12 14],2),...
%     '-','Color',[green-[0.15*(run-1) 0.35*(run-1) 0.15*(run-1)] targetlinealpha],'LineWidth',targetthickness,'Markersize',targetmarker,'MarkerFaceColor','none');
% plot(model_halfleft([2 4 6 8 10 12 14],1),model_halfleft([2 4 6 8 10 12 14],2) + plotshift_halfleft,...
%     '-','Color',[blue-[0.1*(run-1) 0.4*(run-1) 0.45*(run-1)] targetlinealpha],'LineWidth',targetthickness,'Markersize',targetmarker,'MarkerFaceColor','none')
% plot(model_fullleft([2 4 6 8 10 12 14],1),model_fullleft([2 4 6 8 10 12 14],2) + plotshift_fullleft,...
%     '-','Color',[red-[0.4 0.1*(run-1) 0.1*(run-1)] targetlinealpha],'LineWidth',targetthickness,'Markersize',targetmarker,'MarkerFaceColor','none')


% data
runthickness = 3;
runmarker = 2.5;
for run = 3:-1:1
    plot(centre_straight_rotated(:,1,run),centre_straight_rotated(:,2,run),...
        'o','Color',green+[0.25*(run-1) 0.1*(run-1) 0.25*(run-1)],...
        'LineWidth',runthickness,'Markersize',runmarker)
    plot(centre_halfleft_rotated(:,1,run),centre_halfleft_rotated(:,2,run),...
        'o','Color',blue+[0.25*(run-1) 0.15*(run-1) 0.05*(run-1)],...
        'LineWidth',runthickness,'Markersize',runmarker)
    plot(centre_fullleft_rotated(:,1,run),centre_fullleft_rotated(:,2,run),...
        'o','Color',red+[0 0.25*(run-1) 0.25*(run-1)],...
        'LineWidth',runthickness,'Markersize',runmarker)
end

%% cosmetic lines and legend
L1 = plot([-1000 -1000],[-1000 -1000],...
    'x','Color',[0.2 0.2 0.2],'LineWidth',targetthickness,'Markersize',targetmarker,'MarkerFaceColor','none');
L3 = plot([-1000 -1000],[-1000 -1000],...
    'o','Color',[0.4 0.4 0.4],'LineWidth',runthickness,'Markersize',runmarker);
L4 = plot([-1000 -1000],[-1000 -1000],'o','Color',green,...
        'LineWidth',runthickness,'Markersize',runmarker);
L5 = plot([-1000 -1000],[-1000 -1000],'o','Color',blue,...
        'LineWidth',runthickness,'Markersize',runmarker);
L6 = plot([-1000 -1000],[-1000 -1000],'o','Color',red,...
        'LineWidth',runthickness,'Markersize',runmarker);
[lh] = legend([L1 L2 L3 L4 L5 L6], {'Model trajectories','Module outlines','Experiment runs','Straight','50% left','100% left'},...
    'FontSize',16, 'LineWidth',1,'Location','northwest','NumColumns',2);


fh = findall(0,'Type','Figure');
txt_obj = findall(fh,'Type','text');
set(txt_obj,'FontName','Arial');


print(fig, '-painters', 'Track_Analysis_01.eps', '-depsc')

printeps(1,'Track_Analysis_01test');
