from sklearn.svm import LinearSVC
from sklearn.pipeline import make_pipeline
from sklearn.preprocessing import StandardScaler    
from sklearn.model_selection import train_test_split
import os

def main():

    x, x_core = [], []
    y, y_core = [], []

    loadData(x,y,x_core,y_core)

    #singleTest(x,y,x_core,y_core,c=.125)
    multiTest(x,y,x_core,y_core,cFunc=(lambda x: .05 + x * .0125),c_range=1,iterations=10)

# '../training_set/data'
def loadData(x,y,x_core,y_core,threshold=1e-4):
    
    for root, dirs, files in os.walk('../training_set/data/genres'):
        for name in files:

            vals = []

            with open(os.path.join(root,name)) as file:
                line = file.readline().split(',')
                y.append(int(line[1]))
                for i in range(int(line[0])):
                    val = float(file.readline().split(',')[1])
                    vals.append(val if val > threshold else 0)

            x.append(vals)


    for root, dirs, files in os.walk('../training_set/data/core'):
        for name in files:

            vals = []

            with open(os.path.join(root,name)) as file:
                line = file.readline().split(',')
                y_core.append(int(line[1]))
                for i in range(int(line[0])):
                    val = float(file.readline().split(',')[1])
                    vals.append(val if val > threshold else 0)

            x_core.append(vals)


def trainModel(x,y,x_core,y_core,c, core_weight=15):

    x_train, x_test, y_train, y_test = train_test_split(
                x, y, test_size=0.05)       

    weights = [1] * len(x_train) + [core_weight] * len(x_core)

    x_train+=x_core
    y_train+=y_core                                     

    clf = make_pipeline(StandardScaler(),
                        LinearSVC(tol=1e-5, C=c, penalty='l2', multi_class='ovr'))

    clf.fit(x_train,y_train, linearsvc__sample_weight=weights)

    return clf, x_train, y_train, x_test, y_test


def singleTest(x,y,x_core,y_core,c):

    clf, x_train, y_train, x_test, y_test = trainModel(x,y,x_core,y_core,c)

    print("Expected:")
    print(y_test)
    print("Predictions:")
    print(clf.predict(x_test))
    print('Accuracy:')
    print(clf.score(x_test,y_test))
    print('Accuracy on training set:')
    print(clf.score(x_train,y_train))


def multiTest(x,y,x_core,y_core,cFunc,c_range:int,iterations:int=5):
    
    cmap = [] 

    for c_kernel in range(c_range):
        avg_accuracy = 0
        avg_training_accuracy = 0

        for i in range(iterations):

            clf, x_train,y_train,x_test,y_test = trainModel(x,y,x_core,y_core,cFunc(c_kernel))

            avg_accuracy += clf.score(x_test,y_test)
            avg_training_accuracy += clf.score(x_train,y_train)

        avg_accuracy/=iterations
        avg_training_accuracy/=iterations        

        cmap.append([cFunc(c_kernel),avg_accuracy,avg_training_accuracy])

    for row in cmap:
        print(row)

main()