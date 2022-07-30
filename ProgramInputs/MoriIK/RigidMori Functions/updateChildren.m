function updateChildren(body) %TODO change body to name?
%UPDATECHILDREN updates the joint transforms & collision meshes of the specified body's children 
    numChildren = length(body.ChildLocation); %Check how many children the body has %TODO is relying on ChildLocation safe?
    [x, y, theta1, theta2] = deal(body.xyt1t2{:});
    for lv = 1:numChildren
        pos = body.ChildLocation(lv);
        if pos == 3
            tform = trvec2tform([x, y, 0])*axang2tform([0 0 1 -theta2]); %translation and rotation combined in single transform
        else
            tform = axang2tform([0 0 1 theta1]);
        end
        hCurrentChild = body.getChild(lv);
        setFixedTransform(hCurrentChild.Joint, tform); %NOTE "You must create a new Joint object and use replaceJoint to ensure the downstream body geometry is unaffected."
        %Update edge length and xyt1t2t of children automatically %TODO call the following lines seperately outside of function? Only required for body, parent and children.
        if  (hCurrentChild.EdgeLengths(1) ~= body.EdgeLengths(pos))
            hCurrentChild.EdgeLengths(1) = body.EdgeLengths(pos);
            [hCurrentChild.xyt1t2{:}] = solveTriangle(hCurrentChild.EdgeLengths);
            % Set updateColMesh flag on hCurrentChild
            hCurrentChild.updateColMesh = true;
        end
    end
end