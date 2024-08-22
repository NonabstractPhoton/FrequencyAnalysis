from sklearn.svm import LinearSVC
from sklearn.pipeline import make_pipeline
from sklearn.preprocessing import StandardScaler    
from sklearn.model_selection import train_test_split
import os

def main():

    x = []
    y = []

    loadData(x,y,threshold=1e-4)

    singleTest(x,y,.125)
    # multiTest(x,y,cFunc=(lambda x: .005 + .005*x), c_range=50)

# '../training_set/data'
def loadData(x,y,threshold):
    for root, dirs, files in os.walk('../training_set/data'):
        for name in files:

            vals = []

            with open(os.path.join(root,name)) as file:
                line = file.readline().split(',')
                y.append(int(line[1]))
                for i in range(int(line[0])):
                    val = float(file.readline().split(',')[1])
                    vals.append(val if val > threshold else 0)

            x.append(vals)


def singleTest(x,y,c):

    x_train, x_test, y_train, y_test = train_test_split(
                x, y, test_size=0.05)                                            

    clf = make_pipeline(StandardScaler(),
                        LinearSVC(tol=1e-5, C=c, penalty='l2'))

    clf.fit(x_train,y_train)

    print("Expected:")
    print(y_test)
    print("Predictions:")
    print(clf.predict(x_test))
    print('Accuracy:')
    print(clf.score(x_test,y_test))
    print('Accuracy on training set:')
    print(clf.score(x_train,y_train))


def multiTest(x,y,cFunc,c_range):
    cmap = [] 

    for c in range(c_range):
        avg_accuracy = 0
        avg_training_accuracy = 0

        iterations = 10

        for i in range(iterations):

            x_train, x_test, y_train, y_test = train_test_split(
                x, y, test_size=0.05)
                                                

            clf = make_pipeline(StandardScaler(),
                                LinearSVC(tol=1e-5, C=cFunc(c), penalty='l2'))


            clf.fit(x_train,y_train)

            avg_accuracy += clf.score(x_test,y_test)
            avg_training_accuracy += clf.score(x_train,y_train)

        avg_accuracy/=iterations
        avg_training_accuracy/=iterations        

        cmap.append([cFunc(c),avg_accuracy,avg_training_accuracy])

    for row in cmap:
        print(row)

main()