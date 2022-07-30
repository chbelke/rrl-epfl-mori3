function actuateAllEdges(rbtree, edgeLengthMatrix)
%ACTUATEALLEDGES Sets all edges of a rigidMoriTree to specified edge extension values
%   rbtree: rigidBodyTree object
%   edgeLengthMatrix [numModules x 3] matrix with edge extensions

    Bodies = rbtree.Bodies;
    numModules = rbtree.NumBodies;
    
    assert(all(size(edgeLengthMatrix) == [numModules, 3]), 'incorrect edgeLengthMatrix dimensions')
    
    for lv = 1:1:numModules
        actuateEdges(Bodies{lv}.Name, edgeLengthMatrix(lv,:), rbtree);
        updateCollisionMesh(Bodies{lv}); %update collision meshes of all bodies in rbtree
    end
    
    % Update all the collision meshes
    updateCollisionMeshes(Bodies,numModules);
end

