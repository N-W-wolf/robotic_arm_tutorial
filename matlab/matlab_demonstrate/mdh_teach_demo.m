function mdh_teach_demo()
%MDH_TEACH_DEMO Native RTB modified-DH teaching demo.

    close all;

    % RTB Link parameters: [theta d a alpha]
    L(1) = Link([0 0.12 0.00 0.00], 'modified');
    L(2) = Link([0 0.00 0.00 pi/2], 'modified');
    L(3) = Link([0 0.00 0.18 0.00], 'modified');
    L(4) = Link([0 0.00 0.18 0.00], 'modified');

    robot = SerialLink(L, 'name', 'MDH Demo');

    q0 = [0 0 0 0];

    robot.plot(q0, ...
        'workspace', [-0.25 0.6 -0.25 0.6 0.00 0.60], ...
        'floorlevel', 0, ...
        'jointdiam', 1.2);

    title('Modified DH robot');

    % Peter Corke Robotics Toolbox native teaching interface.  The callback
    % prints the current transform chain whenever a teach slider changes.
    robot.teach(q0, 'rpy/xyz', 'callback', @printTransforms);
end

function printTransforms(robot, q)
    fprintf('\n========================================\n');
    fprintf('q = [% .6f % .6f % .6f % .6f] rad\n', q);

    T = eye(4);
    for i = 1:robot.n
        Ai = robot.links(i).A(q(i));
        Ai = Ai.T;
        T = T * Ai;

        fprintf('\nA%d =\n', i);
        disp(Ai);
    end

    fprintf('T04 = A1 * A2 * A3 * A4 =\n');
    disp(T);
    fprintf('========================================\n');
end
