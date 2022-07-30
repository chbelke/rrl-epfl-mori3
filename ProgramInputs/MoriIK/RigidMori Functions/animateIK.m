function animateIK(rbtree, trajectory)
%ANIMATEIK produce a .avi video file animating the trajectory
    fig = figure('color',[1 1 1],'units','pixels','position', [160,90,1920,1080]);
    fig.Resize = 'off';
    fig.GraphicsSmoothing = 'on';
    
    writerObj = VideoWriter(sprintf('Animations/%s.avi', datestr(now,'yyyy-mm-dd_HH-MM')));
    writerObj.FrameRate = 30;
    open(writerObj);
    
    % Set the edge extensions of all modules to the initial value
    actuateAllEdges(rbtree, trajectory.edgeLengthMatrix(:,:,1))
    
    ax = show(rbtree, trajectory.configs(1,:), 'Collisions','on','Visuals','off');
    ax.LineWidth = 2; ax.FontSize = 13;
    %axis equal,
    hold on, grid on; view(-30,45);
    xlim([-0.7 1.1]);
    ylim([-0.9 0.9]);
    zlim([-0.5 1.3]);
    
    if isfield(trajectory, 'targetTraj')
       x = squeeze(trajectory.targetTraj(1,4,:));
       y = squeeze(trajectory.targetTraj(2,4,:));
       z = squeeze(trajectory.targetTraj(3,4,:));
       plot3(x,y,z);
       u = squeeze(trajectory.targetTraj(1,3,:));
       v = squeeze(trajectory.targetTraj(2,3,:));
       w = squeeze(trajectory.targetTraj(3,3,:));
       quiver3(x,y,z,u,v,w)
    elseif isfield(trajectory, 'target')
        quiver3(trajectory.target(1,4)*ones(3,1), trajectory.target(2,4)*ones(3,1), trajectory.target(3,4)*ones(3,1),...
            trajectory.target(1,1:3)', trajectory.target(2,1:3)',trajectory.target(3,1:3)', 0.2, 'r');
        drawnow
    end
    
    for lv = 1:90 %generate 3 seconds of still video
        frame = getframe(fig);
        writeVideo(writerObj, frame);
    end
    
    for lv = 2:trajectory.iterations+1
        
        actuateAllEdges(rbtree, trajectory.edgeLengthMatrix(:,:,lv))

        show(rbtree, trajectory.configs(lv,:), 'Collisions','on','Visuals','off','PreservePlot', false);
        drawnow
        frame = getframe(fig);
        writeVideo(writerObj, frame);
    end
    
    %% End animation with rotating view around the final configuration
    for lv = 1:15
        frame = getframe(fig);
        writeVideo(writerObj,frame);
    end

    for v = -30:1:60
        view(v,abs(abs(v-60)/2));
        frame = getframe(fig);
        writeVideo(writerObj,frame);
    end

    for v = [61:1:180 -179:1:-75]
        view(v,0);
        frame = getframe(fig);
        writeVideo(writerObj,frame);
    end

    for v = -74:1:-30
        view(v,75+v);
        frame = getframe(fig);
        writeVideo(writerObj,frame);
    end
    
    hold off
    close(writerObj);
end
