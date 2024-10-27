import java.util.*;

public class main {

    public static void main(String[] args) {

        solver();

    }



    public static int solver() {
        boolean found = false;
        LinkedList<ProbState> fringe = new LinkedList<>();

        ProbState temp = null;

        ProbState init = new ProbState(3,3,1);
        ProbState goal = new ProbState(0,0,0);
        ProbState curState;
        ProbState fin = null;

        ProbState moveA = new ProbState(1,0,1);
        ProbState moveB = new ProbState(2,0,1);
        ProbState moveC = new ProbState(0,1,1);
        ProbState moveD = new ProbState(0,2,1);
        ProbState moveE = new ProbState(1,1,1);
        ProbState[] possibleMoves = {moveA,moveB,moveC,moveD,moveE};


        fringe.add(init);

        while (!found) {

            if(fringe.isEmpty()){
                found = true;
                System.out.println("No solution found");
                break;
            } else {
               curState = fringe.poll();
            }

            if (curState.numCan == 0 && curState.numMiss == 0 && curState.side == 0) {
                found = true;
                System.out.println("Solution found");
                fin = curState;
                break;
            }

            for (ProbState possibleMove : possibleMoves) {

                if (curState.side == 1) {

                    int newNumMiss = curState.numMiss - possibleMove.numMiss;
                    int newNumCan = curState.numCan - possibleMove.numCan;

                    if (newNumMiss >= newNumCan || newNumMiss == 0) {
                        if (newNumMiss >= 0 && newNumCan >= 0 && newNumMiss <= 3 && newNumCan <= 3) {
                            temp = curState.subState(possibleMove);
                            if (temp != null) {
                                temp.setParent(curState);
                                fringe.add(temp);
                            }
                        }
                    }


                } else {

                    int newNumMiss = curState.numMiss + possibleMove.numMiss;
                    int newNumCan = curState.numCan + possibleMove.numCan;

                    if (newNumMiss >= newNumCan || newNumMiss == 3) {
                        if (newNumMiss >= 0 && newNumCan >= 0 && newNumMiss <= 3 && newNumCan <= 3) {
                            temp = curState.addState(possibleMove);
                            if (temp != null) {
                                temp.setParent(curState);
                                fringe.add(temp);
                            }
                        }
                    }

                }
            }

        }


        if (fin != null) {
            ProbState parent = fin.getParent();
            System.out.println("Parents of the final state:");
            System.out.println("0 0");
            while (parent != null) {
                System.out.print(parent.numMiss);
                System.out.print(" " + parent.numCan);
                System.out.println();
                parent = parent.getParent();
            }
        }


       return 0;
    }


}

