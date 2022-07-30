%% Data
clear
close all

load('HexUpData.mat')

totalCols = 16144;
cols = turbo(10*totalCols)-0.05;


%% Model
maxEL = 192;
minEL = 180;
LInt = 0.001;

hVals3 = zeros(((maxEL-minEL)/LInt)*2,4);
hVals4 = zeros(((maxEL-minEL)/LInt)*2,4);
hVals5 = zeros(((maxEL-minEL)/LInt)*2,4);
hVals6 = zeros(((maxEL-minEL)/LInt)*2,4);

modules = zeros(3,3,2);

% THREE
l = 0;
for j = maxEL:-LInt:minEL %ext
    l = l+1;
    hVals3(l,1) = j;
    hVals3(l,2) = minEL;
    baseIntEdge = j/(2*sin(pi/3));
    if baseIntEdge > minEL
        hVals3(l,3) = NaN;
    else
        hVals3(l,3) = sqrt(minEL^2 - baseIntEdge^2);
        
        triangleHeight = sqrt(baseIntEdge^2-(j/2)^2);
        extEdgeAngle = (2*pi+(3-4)*pi)/3;
        modules(:,:,1)=[-j/2,j/2,0;...
            -triangleHeight,-triangleHeight,0;...
            0,0,hVals3(l,3)];
        modules(:,:,2)=[modules(1,2,1),modules(1,2,1)+j*cos((pi-extEdgeAngle)*(1)),0;...
                modules(2,2,1),modules(2,2,1)+j*sin((pi-extEdgeAngle)*(1)),0;...
                0,0,hVals3(l,3)];
        P1 = cross(modules(:,2,1)-modules(:,1,1),modules(:,3,1)-modules(:,2,1));
        P2 = cross(modules(:,2,2)-modules(:,1,2),modules(:,3,2)-modules(:,2,2));
        dihedral = acos(abs(P1(1)*P2(1) + P1(2)*P2(2) + P1(3)*P2(3)) / ...
            (sqrt(P1(1)^2 + P1(2)^2 + P1(3)^2) * sqrt(P2(1)^2 + P2(2)^2 + P2(3)^2)));
        hVals3(l,4) = 180-rad2deg(dihedral);
    end
end
for k = minEL+LInt:LInt:maxEL %int
    l = l+1;
    hVals3(l,1) = minEL;
    hVals3(l,2) = k;
    baseIntEdge = j/(2*sin(pi/3));
    if baseIntEdge > k
        hVals3(l,3) = NaN;
    else
        hVals3(l,3) = sqrt(k^2 - baseIntEdge^2);
        
        triangleHeight = sqrt(baseIntEdge^2-(j/2)^2);
        extEdgeAngle = (2*pi+(3-4)*pi)/3;
        modules(:,:,1)=[-j/2,j/2,0;...
            -triangleHeight,-triangleHeight,0;...
            0,0,hVals3(l,3)];
        modules(:,:,2)=[modules(1,2,1),modules(1,2,1)+j*cos((pi-extEdgeAngle)*(1)),0;...
                modules(2,2,1),modules(2,2,1)+j*sin((pi-extEdgeAngle)*(1)),0;...
                0,0,hVals3(l,3)];
        P1 = cross(modules(:,2,1)-modules(:,1,1),modules(:,3,1)-modules(:,2,1));
        P2 = cross(modules(:,2,2)-modules(:,1,2),modules(:,3,2)-modules(:,2,2));
        dihedral = acos(abs(P1(1)*P2(1) + P1(2)*P2(2) + P1(3)*P2(3)) / ...
            (sqrt(P1(1)^2 + P1(2)^2 + P1(3)^2) * sqrt(P2(1)^2 + P2(2)^2 + P2(3)^2)));
        hVals3(l,4) = 180-rad2deg(dihedral);
    end
end

% four
l = 0;
for j = maxEL:-LInt:minEL %ext
    l = l+1;
    hVals4(l,1) = j;
    hVals4(l,2) = minEL;
    baseIntEdge = j/(2*sin(pi/4));
    if baseIntEdge > minEL
        hVals4(l,3) = NaN;
    else
        hVals4(l,3) = sqrt(minEL^2 - baseIntEdge^2);
        
        triangleHeight = sqrt(baseIntEdge^2-(j/2)^2);
        extEdgeAngle = (2*pi+(4-4)*pi)/4;
        modules(:,:,1)=[-j/2,j/2,0;...
            -triangleHeight,-triangleHeight,0;...
            0,0,hVals4(l,3)];
        modules(:,:,2)=[modules(1,2,1),modules(1,2,1)+j*cos((pi-extEdgeAngle)*(1)),0;...
                modules(2,2,1),modules(2,2,1)+j*sin((pi-extEdgeAngle)*(1)),0;...
                0,0,hVals4(l,3)];
        P1 = cross(modules(:,2,1)-modules(:,1,1),modules(:,3,1)-modules(:,2,1));
        P2 = cross(modules(:,2,2)-modules(:,1,2),modules(:,3,2)-modules(:,2,2));
        dihedral = acos(abs(P1(1)*P2(1) + P1(2)*P2(2) + P1(3)*P2(3)) / ...
            (sqrt(P1(1)^2 + P1(2)^2 + P1(3)^2) * sqrt(P2(1)^2 + P2(2)^2 + P2(3)^2)));
        hVals4(l,4) = rad2deg(dihedral);
    end
end
for k = minEL+LInt:LInt:maxEL %int
    l = l+1;
    hVals4(l,1) = minEL;
    hVals4(l,2) = k;
    baseIntEdge = j/(2*sin(pi/4));
    if baseIntEdge > k
        hVals4(l,3) = NaN;
    else
        hVals4(l,3) = sqrt(k^2 - baseIntEdge^2);
        
        triangleHeight = sqrt(baseIntEdge^2-(j/2)^2);
        extEdgeAngle = (2*pi+(4-4)*pi)/4;
        modules(:,:,1)=[-j/2,j/2,0;...
            -triangleHeight,-triangleHeight,0;...
            0,0,hVals4(l,3)];
        modules(:,:,2)=[modules(1,2,1),modules(1,2,1)+j*cos((pi-extEdgeAngle)*(1)),0;...
                modules(2,2,1),modules(2,2,1)+j*sin((pi-extEdgeAngle)*(1)),0;...
                0,0,hVals4(l,3)];
        P1 = cross(modules(:,2,1)-modules(:,1,1),modules(:,3,1)-modules(:,2,1));
        P2 = cross(modules(:,2,2)-modules(:,1,2),modules(:,3,2)-modules(:,2,2));
        dihedral = acos(abs(P1(1)*P2(1) + P1(2)*P2(2) + P1(3)*P2(3)) / ...
            (sqrt(P1(1)^2 + P1(2)^2 + P1(3)^2) * sqrt(P2(1)^2 + P2(2)^2 + P2(3)^2)));
        hVals4(l,4) = rad2deg(dihedral);
    end
end


% five
l = 0;
for j = maxEL:-LInt:minEL %ext
    l = l+1;
    hVals5(l,1) = j;
    hVals5(l,2) = minEL;
    baseIntEdge = j/(2*sin(pi/5));
    if baseIntEdge > minEL
        hVals5(l,3) = NaN;
    else
        hVals5(l,3) = sqrt(minEL^2 - baseIntEdge^2);
        
        triangleHeight = sqrt(baseIntEdge^2-(j/2)^2);
        extEdgeAngle = (2*pi+(5-4)*pi)/5;
        modules(:,:,1)=[-j/2,j/2,0;...
            -triangleHeight,-triangleHeight,0;...
            0,0,hVals5(l,3)];
        modules(:,:,2)=[modules(1,2,1),modules(1,2,1)+j*cos((pi-extEdgeAngle)*(1)),0;...
                modules(2,2,1),modules(2,2,1)+j*sin((pi-extEdgeAngle)*(1)),0;...
                0,0,hVals5(l,3)];
        P1 = cross(modules(:,2,1)-modules(:,1,1),modules(:,3,1)-modules(:,2,1));
        P2 = cross(modules(:,2,2)-modules(:,1,2),modules(:,3,2)-modules(:,2,2));
        dihedral = acos(abs(P1(1)*P2(1) + P1(2)*P2(2) + P1(3)*P2(3)) / ...
            (sqrt(P1(1)^2 + P1(2)^2 + P1(3)^2) * sqrt(P2(1)^2 + P2(2)^2 + P2(3)^2)));
        hVals5(l,4) = rad2deg(dihedral);
    end
end
for k = minEL+LInt:LInt:maxEL %int
    l = l+1;
    hVals5(l,1) = minEL;
    hVals5(l,2) = k;
    baseIntEdge = j/(2*sin(pi/5));
    if baseIntEdge > k
        hVals5(l,3) = NaN;
    else
        hVals5(l,3) = sqrt(k^2 - baseIntEdge^2);
        
        triangleHeight = sqrt(baseIntEdge^2-(j/2)^2);
        extEdgeAngle = (2*pi+(5-4)*pi)/5;
        modules(:,:,1)=[-j/2,j/2,0;...
            -triangleHeight,-triangleHeight,0;...
            0,0,hVals5(l,3)];
        modules(:,:,2)=[modules(1,2,1),modules(1,2,1)+j*cos((pi-extEdgeAngle)*(1)),0;...
                modules(2,2,1),modules(2,2,1)+j*sin((pi-extEdgeAngle)*(1)),0;...
                0,0,hVals5(l,3)];
        P1 = cross(modules(:,2,1)-modules(:,1,1),modules(:,3,1)-modules(:,2,1));
        P2 = cross(modules(:,2,2)-modules(:,1,2),modules(:,3,2)-modules(:,2,2));
        dihedral = acos(abs(P1(1)*P2(1) + P1(2)*P2(2) + P1(3)*P2(3)) / ...
            (sqrt(P1(1)^2 + P1(2)^2 + P1(3)^2) * sqrt(P2(1)^2 + P2(2)^2 + P2(3)^2)));
        hVals5(l,4) = rad2deg(dihedral);
    end
end

% six
l = 0;
for j = maxEL:-LInt:minEL %ext
    l = l+1;
    hVals6(l,1) = j;
    hVals6(l,2) = minEL;
    baseIntEdge = j/(2*sin(pi/6));
    if baseIntEdge > minEL
        hVals6(l,3) = NaN;
    else
        hVals6(l,3) = sqrt(minEL^2 - baseIntEdge^2);
        
        triangleHeight = sqrt(baseIntEdge^2-(j/2)^2);
        extEdgeAngle = (2*pi+(6-4)*pi)/6;
        modules(:,:,1)=[-j/2,j/2,0;...
            -triangleHeight,-triangleHeight,0;...
            0,0,hVals6(l,3)];
        modules(:,:,2)=[modules(1,2,1),modules(1,2,1)+j*cos((pi-extEdgeAngle)*(1)),0;...
                modules(2,2,1),modules(2,2,1)+j*sin((pi-extEdgeAngle)*(1)),0;...
                0,0,hVals6(l,3)];
        P1 = cross(modules(:,2,1)-modules(:,1,1),modules(:,3,1)-modules(:,2,1));
        P2 = cross(modules(:,2,2)-modules(:,1,2),modules(:,3,2)-modules(:,2,2));
        dihedral = acos(abs(P1(1)*P2(1) + P1(2)*P2(2) + P1(3)*P2(3)) / ...
            (sqrt(P1(1)^2 + P1(2)^2 + P1(3)^2) * sqrt(P2(1)^2 + P2(2)^2 + P2(3)^2)));
        hVals6(l,4) = rad2deg(dihedral);
    end
end
for k = minEL+LInt:LInt:maxEL %int
    l = l+1;
    hVals6(l,1) = minEL;
    hVals6(l,2) = k;
    baseIntEdge = j/(2*sin(pi/6));
    if baseIntEdge > k
        hVals6(l,3) = NaN;
    else
        hVals6(l,3) = sqrt(k^2 - baseIntEdge^2);
        triangleHeight = sqrt(baseIntEdge^2-(j/2)^2);
        extEdgeAngle = (2*pi+(6-4)*pi)/6;
        modules(:,:,1)=[-j/2,j/2,0;...
            -triangleHeight,-triangleHeight,0;...
            0,0,hVals6(l,3)];
        modules(:,:,2)=[modules(1,2,1),modules(1,2,1)+j*cos((pi-extEdgeAngle)*(1)),0;...
                modules(2,2,1),modules(2,2,1)+j*sin((pi-extEdgeAngle)*(1)),0;...
                0,0,hVals6(l,3)];
        P1 = cross(modules(:,2,1)-modules(:,1,1),modules(:,3,1)-modules(:,2,1));
        P2 = cross(modules(:,2,2)-modules(:,1,2),modules(:,3,2)-modules(:,2,2));
        dihedral = acos(abs(P1(1)*P2(1) + P1(2)*P2(2) + P1(3)*P2(3)) / ...
            (sqrt(P1(1)^2 + P1(2)^2 + P1(3)^2) * sqrt(P2(1)^2 + P2(2)^2 + P2(3)^2)));
        hVals6(l,4) = rad2deg(dihedral);
    end
end

hVals3R = rmmissing(hVals3);
hVals4R = rmmissing(hVals4);
hVals5R = rmmissing(hVals5);
hVals6R = rmmissing(hVals6);

hVals3R = sortrows(hVals3R,3);
hVals4R = sortrows(hVals4R,3);
hVals5R = sortrows(hVals5R,3);
hVals6R = sortrows(hVals6R,3);
mVals3 = linspace(3.499,2.5,length(hVals3R));
mVals4 = linspace(4.499,3.5,length(hVals4R));
mVals5 = linspace(5.499,4.5,length(hVals5R));
% mVals6 = linspace(5.5+0.999*length(hVals6R)/length(hVals5R),5.5,length(hVals6R));
mVals6 = linspace(6.499,5.5,length(hVals6R));

%% Plot

fig = figure('color',[1 1 1],'units','normalized','position', [.1,.1,.7,.8]);
fig.Resize = 'off';
set(fig,'defaultAxesColorOrder',[0 0 0; 0.6 0.2 0.1]);
axes('Linewidth',2,'FontSize',15);

hold on, set(gca, 'XDir','reverse'), set(gca, 'FontName', 'Arial')
yyaxis left
xlim([2.25 6.75])
ylim([0 200])
xticks([3 4 5 6]);
set(gca,'XTickLabels',{'\fontsize{18}{3}','\fontsize{18}{4}','\fontsize{18}{5}','\fontsize{18}{6}'})
yticks([20 40 60 80 100 120 140 160 180 200]);
set(gca,'TickLength',[0.005 0])
xlabel('No. of modules','FontSize',22);
ylabel('Height / Edge length (mm)','FontSize',20);

yyaxis right
ylim([0 130])
yticks([20 40 60 80 100 120]);
ylabel(sprintf('Dihedral angle (%c)',char(176)),'FontSize',20);

yyaxis left
for m = 10:10:200
    plot([2.26 6.74],[m m],'-','Color',[0.95 0.95 0.95],'LineWidth',1)
end
for m = 1:5
    plot([m+1.5 m+1.5],[0.5 200],'-','Color',[0.85 0.85 0.85],'LineWidth',2)
end

%%%%% 3 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
linewidthMain = 4;
linewidthScnd = 2.5;
yyaxis left
l1 = plot(mVals3,hVals3R(:,1),'-','Color',[0.51 0.37 0.79],'LineWidth',linewidthScnd);
l2 = plot(mVals3,hVals3R(:,2),'-','Color',[0.35 0.58 0.72],'LineWidth',linewidthScnd);

hCols3 = cols(totalCols+7*round(hVals3R(:,3)*100),:);
z = zeros(size(mVals3));
hCols3D = reshape(hCols3,[1,length(mVals3),3]);
surface([mVals3;mVals3],[transpose(hVals3R(:,3));transpose(hVals3R(:,3))],[z;z],...
    [hCols3D;hCols3D],...
    'EdgeColor','interp',...
    'linew',linewidthMain)
%l3 = plot(mVals3,hVals3R(:,3),'-','Color',[0.2 0.6 0.2],'LineWidth',linewidthMain);
l3 = scatter([0],[0]);
alpha(l3,0);

mVals3Data = mVals3([1,12001,15801,19863,24001]);
errorbar(mVals3Data,ThreeUpData(:,1),ThreeUpDataSTD(:,1),'o',...
    'MarkerSize',4,'LineWidth',linewidthScnd/2,...
    'MarkerFaceColor',[0.3 0.1 0.7],'Color',[0.3 0.1 0.7])
errorbar(mVals3Data,ThreeUpData(:,2),ThreeUpDataSTD(:,2),'o',...
    'MarkerSize',4,'LineWidth',linewidthScnd/2,...
    'MarkerFaceColor',[0.1 0.4 0.6],'Color',[0.1 0.4 0.6])

yyaxis right
l4 = plot(mVals3,hVals3R(:,4),'-','Color',[0.83 0.31 0.22],'LineWidth',linewidthScnd);
ThreeUpData(:,4) = ThreeUpData(:,4)-180;
errorbar(mVals3Data,ThreeUpData(:,4),ThreeUpDataSTD(:,4),'o',...
    'MarkerSize',4,'LineWidth',linewidthScnd/2,...
    'MarkerFaceColor',[0.7 0.2 0.1],'Color',[0.7 0.2 0.1])

%%%%% 4 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
yyaxis left
plot(mVals4,hVals4R(:,1),'-','Color',[0.51 0.37 0.79],'LineWidth',linewidthScnd)
plot(mVals4,hVals4R(:,2),'-','Color',[0.35 0.58 0.72],'LineWidth',linewidthScnd)

hCols4 = cols(totalCols+7*round(hVals4R(:,3)*100),:);
z = zeros(size(mVals4));
hCols4D = reshape(hCols4,[1,length(mVals4),3]);
surface([mVals4;mVals4],[transpose(hVals4R(:,3));transpose(hVals4R(:,3))],[z;z],...
    [hCols4D;hCols4D],...
    'EdgeColor','interp',...
    'linew',linewidthMain)
%plot(mVals4,hVals4R(:,3),'-','Color',[0.2 0.6 0.2],'LineWidth',linewidthMain)

mVals4Data = mVals4([1,6544,12001,16525,20103,24001]);
errorbar(mVals4Data,FourUpData(:,1),FourUpDataSTD(:,1),'o',...
    'MarkerSize',4,'LineWidth',linewidthScnd/2,...
    'MarkerFaceColor',[0.3 0.1 0.7],'Color',[0.3 0.1 0.7])
errorbar(mVals4Data,FourUpData(:,2),FourUpDataSTD(:,2),'o',...
    'MarkerSize',4,'LineWidth',linewidthScnd/2,...
    'MarkerFaceColor',[0.1 0.4 0.6],'Color',[0.1 0.4 0.6])

yyaxis right
plot(mVals4,hVals4R(:,4),'-','Color',[0.83 0.31 0.22],'LineWidth',linewidthScnd)
FourUpData(:,4) = FourUpData(:,4)-180;
errorbar(mVals4Data,FourUpData(:,4),FourUpDataSTD(:,4),'o',...
    'MarkerSize',4,'LineWidth',linewidthScnd/2,...
    'MarkerFaceColor',[0.7 0.2 0.1],'Color',[0.7 0.2 0.1])

%%%%% 5 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
yyaxis left
plot(mVals5,hVals5R(:,1),'-','Color',[0.51 0.37 0.79],'LineWidth',linewidthScnd)
plot(mVals5,hVals5R(:,2),'-','Color',[0.35 0.58 0.72],'LineWidth',linewidthScnd)

hCols5 = cols(totalCols+7*round(hVals5R(:,3)*100),:);
z = zeros(size(mVals5));
hCols5D = reshape(hCols5,[1,length(mVals5),3]);
surface([mVals5;mVals5],[transpose(hVals5R(:,3));transpose(hVals5R(:,3))],[z;z],...
    [hCols5D;hCols5D],...
    'EdgeColor','interp',...
    'linew',linewidthMain)
%plot(mVals5,hVals5R(:,3),'-','Color',[0.2 0.6 0.2],'LineWidth',linewidthMain)

mVals5Data = mVals5([1,2798,5921,9225,12001,15209,18002,20885,24001]);
errorbar(mVals5Data,FiveUpData(:,1),FiveUpDataSTD(:,1),'o',...
    'MarkerSize',4,'LineWidth',linewidthScnd/2,...
    'MarkerFaceColor',[0.3 0.1 0.7],'Color',[0.3 0.1 0.7])
errorbar(mVals5Data,FiveUpData(:,2),FiveUpDataSTD(:,2),'o',...
    'MarkerSize',4,'LineWidth',linewidthScnd/2,...
    'MarkerFaceColor',[0.1 0.4 0.6],'Color',[0.1 0.4 0.6])

yyaxis right
plot(mVals5,hVals5R(:,4),'-','Color',[0.83 0.31 0.22],'LineWidth',linewidthScnd)
FiveUpData(:,4) = FiveUpData(:,4)-180;
errorbar(mVals5Data,FiveUpData(:,4),FiveUpDataSTD(:,4),'o',...
    'MarkerSize',4,'LineWidth',linewidthScnd/2,...
    'MarkerFaceColor',[0.7 0.2 0.1],'Color',[0.7 0.2 0.1])


%%%%% 6 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
yyaxis left
plot(mVals6,hVals6R(:,1),'-','Color',[0.51 0.37 0.79],'LineWidth',linewidthScnd)
plot(mVals6,hVals6R(:,2),'-','Color',[0.35 0.58 0.72],'LineWidth',linewidthScnd)

hCols6 = cols(totalCols+7*round(hVals6R(:,3)*100),:);
z = zeros(size(mVals6));
hCols6D = reshape(hCols6,[1,length(mVals6),3]);
%plot(mVals6,hVals6R(:,3),'-','Color',[1 1 1],'LineWidth',linewidthMain)
surface([mVals6;mVals6],[transpose(hVals6R(:,3));transpose(hVals6R(:,3))],[z;z],...
    [hCols6D;hCols6D],...
    'EdgeColor','interp',...
    'linew',4)

mVals6Data = mVals6([100,300,701,1198,1798,2600,3506,4500,5711,7004,8392,9991,11996]);
l7 = errorbar(mVals6Data,HexUpData(:,1),HexUpDataSTD(:,1),'o',...
    'MarkerSize',4,'LineWidth',linewidthScnd/2,...
    'MarkerFaceColor',[0.3 0.1 0.7],'Color',[0.3 0.1 0.7]);
l6 = errorbar(mVals6Data,HexUpData(:,2),HexUpDataSTD(:,2),'o',...
    'MarkerSize',4,'LineWidth',linewidthScnd/2,...
    'MarkerFaceColor',[0.1 0.4 0.6],'Color',[0.1 0.4 0.6]);

yyaxis right
plot(mVals6,hVals6R(:,4),'-','Color',[0.83 0.31 0.22],'LineWidth',linewidthScnd)
HexUpData(:,4) = HexUpData(:,4)-180;
l5 = errorbar(mVals6Data,HexUpData(:,4),HexUpDataSTD(:,4),'o',...
    'MarkerSize',4,'LineWidth',linewidthScnd/2,...
    'MarkerFaceColor',[0.7 0.2 0.1],'Color',[0.7 0.2 0.1]);



% target points
yyaxis left
l0 = plot([mVals6Data mVals5Data mVals4Data mVals3Data],hTargetPoints,...
    '+','Color',[0 0 0],'LineWidth',1.5, 'MarkerSize',5);

% line for legend
legLin = flip([4.9:0.001:5.1]);
legCols = turbo(length(legLin)/8*10);
hColsLD = legCols(round(0.1*length(legCols)):round(0.9*length(legCols))-1,:);
hColsLD2 = reshape(hColsLD,[1,length(hColsLD),3]);
z = zeros(size(legLin));
surface([legLin;legLin], ...
    [transpose(5.*ones(length(legLin),1));transpose(5.*ones(length(legLin),1))],...
    [z;z],[hColsLD2;hColsLD2],...
    'EdgeColor','interp',...
    'linew',4)


legend([l3 l4 l2 l1 l0 l5 l6 l7], {'Centre height model','Dihedral angle model',...
    'Internal edge model','External edge model','Target points','Dihedral angle data',...
    'Internal edge data','External edge data'},'FontSize',17,...
    'Position',[0.6075 0.145 0.165 0.165],'LineWidth',1.5,'NumColumns',2)

%print(fig, '-painters', 'SurfaceModelAndData_01.eps', '-depsc')

%myaa(6);