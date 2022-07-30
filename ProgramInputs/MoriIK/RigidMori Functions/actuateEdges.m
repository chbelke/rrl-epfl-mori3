function actuateEdges(name, edgeLengths, rbtree)
%ACTUATEEDGES Actuates the edges of a selected module, updates the module's, its
% childrens' and its parents' edge lengths and collision meshes
%   Inputs:
%       name: name of the rigidBody module to actuate
%       edgeLengths: Edge lengths of the module (m)
%       rbtree: rigidBodyTree object
    body = rbtree.getBody(name);
    %edgeLengths(edgeLengths>0.192) = 0.192; %TODO saturation
    %edgeLengths(edgeLengths<0.186) = 0.186;
    modEdgeLengths = (body.EdgeLengths ~= edgeLengths);
    if any(modEdgeLengths) %If none of the edge lengths were modified do nothing
        body.EdgeLengths = edgeLengths;
        [body.xyt1t2{:}] = solveTriangle(edgeLengths);
        %FIXME Big issue, update parent edgeLength, how do we treat parent=base?
        parentBody = body.Parent;
        if (modEdgeLengths(1) && (~strcmp(parentBody.Name, rbtree.BaseName))) %if we update the first edge extension, update the parent and siblings
            if strcmp(parentBody.getChild(1).Name, name)
                pos = parentBody.ChildLocation(1);
            else
                pos = parentBody.ChildLocation(2);
            end
            parentBody.EdgeLengths(pos) = edgeLengths(1);
            [parentBody.xyt1t2{:}] = solveTriangle(parentBody.EdgeLengths);
            % Set updateColMesh flag on parentBody
            parentBody.updateColMesh = true;
            % Update siblings and self
            updateChildren(parentBody);
        else
            % Set updateColMesh flag on body
            body.updateColMesh = true;
        end

        %Update children
        updateChildren(body);
        %Update grand children
        numChildren = length(body.ChildLocation); %Check how many children the body has %TODO is relying on ChildLocation safe?
        for lv = 1:numChildren
            pos = body.ChildLocation(lv);
            if modEdgeLengths(pos) %if shape of children changed
                updateChildren(body.getChild(lv)); %update grand children
            end
        end
    end
end