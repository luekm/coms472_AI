import java.util.ArrayList;

public class ProbState {

    public ProbState(int m, int c, int s) {

        numMiss = m;
        numCan = c;
        side = s;
        numMoves = 0;

    }
    int numMiss;
    int numCan;
    int side;
    int numMoves;
    ProbState parent;

    public ProbState addState(ProbState a) {


        int m = this.numMiss + a.numMiss;
        int c = this.numCan + a.numCan;
        int s = this.side + a.side;

        return new ProbState(m,c,s);
    }

    public ProbState subState(ProbState a) {

       int m = this.numMiss - a.numMiss;
       int c = this.numCan - a.numCan;
       int s = this.side - a.side;

       return new ProbState(m,c,s);

    }

    public ProbState copyState() {
        ProbState newState = new ProbState(0,0,0);
        newState.numMiss = this.numMiss;
        newState.numCan = this.numCan;
        newState.side = this.side;

        return newState;
    }

    public void setParent(ProbState a) {
        parent = a;
    }

    public ProbState getParent() {
        return this.parent;
    }

    public boolean equals(ProbState a) {

        if(this.numCan == a.numCan && this.numMoves == a.numMoves && this.side == a.side) {
            return true;
        }

        return false;
    }
}
