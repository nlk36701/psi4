#ifndef HAMILTONIAN_H 
#define HAMILTONIAN_H

namespace boost {
template<class T> class shared_ptr;
}

namespace psi {

class Matrix;
class Vector;
class JK;

// => BASE CLASSES <= //

class Hamiltonian {

protected:

    /// Print flag, defaults to 1
    int print_;	
    /// Debug flag, defaults to 0
    int debug_;	
    /// jk object
    boost::shared_ptr<JK> jk_;  

public:
    // => Constructors < = //

    Hamiltonian(boost::shared_ptr<JK> jk);
    /// Destructor
    virtual ~Hamiltonian();

    // => Accessors <= //

    /**
    * Pointer to the JK object
    * @return current JK object
    */
    boost::shared_ptr<JK> jk() const { return jk_; }

    /**
    * Print header information regarding Hamiltonian
    * type on output file
    */
    virtual void print_header() const = 0;

    // => Knobs <= //

    /**
    * Knob to swap out a JK object
    * @param jk new JK object
    */
    void set_JK(boost::shared_ptr<JK> jk) { jk_ = jk; }
    /// Print flag (defaults to 1)
    void set_print(int print) { print_ = print; }
    /// Debug flag (defaults to 0)
    void set_debug(int debug) { debug_ = debug; }
};

class RHamiltonian : public Hamiltonian {

public:
    // => Constructors < = //

    RHamiltonian(boost::shared_ptr<JK> jk);
    /// Destructor
    virtual ~RHamiltonian();

    // => Required Methods <= //

    /**
    * Return the approximate diagonal of the Hamiltonian
    * (typically orbital energy differences). This is used
    * for vector guess, eigenvector occupation, and preconditioning.
    * @return diagonal approximation, blocked by symmetry
    */
    virtual boost::shared_ptr<Vector> diagonal() = 0; 
    /**
    * Form the product Hx for each x found in the argument, placing in second argument 
    * @param x vector of state functions, blocked by symmetry 
    * @param b vector of product functions, blocked by symmetry (preallocated)
    */
    virtual void  product(const std::vector<boost::shared_ptr<Vector> >& x,
                                std::vector<boost::shared_ptr<Vector> >& b) = 0; 

};

class UHamiltonian : public Hamiltonian {

public:
    // => Constructors < = //

    UHamiltonian(boost::shared_ptr<JK> jk);
    /// Destructor
    virtual ~UHamiltonian();

    // => Required Methods <= //

    /**
    * Return the approximate diagonal of the Hamiltonian
    * (typically orbital energy differences). This is used
    * for vector guess, eigenvector occupation, and preconditioning.
    * @return diagonal approximation, \alpha and \beta vectors,
    * blocked by symmetry
    */
    virtual std::pair<boost::shared_ptr<Vector>,
                      boost::shared_ptr<Vector> > diagonal() = 0; 
    /**
    * Form the product Hx for each x found in the argument, placing in second argument 
    * @param x vector of state functions, \alpha and \beta, blocked by symmetry 
    * @param b vector of product functions, \alpha and \beta, blocked by symmetry (preallocated)
    */
    virtual void product(const std::vector<std::pair<boost::shared_ptr<Vector>, boost::shared_ptr<Vector> > >& x,
                               std::vector<std::pair<boost::shared_ptr<Vector>, boost::shared_ptr<Vector> > >& b) = 0; 
};

// => APPLIED CLASSES <= //

class MatrixRHamiltonian : public RHamiltonian {

protected:

    boost::shared_ptr<Matrix> M_;

public:

    MatrixRHamiltonian(boost::shared_ptr<Matrix> M);
    virtual ~MatrixRHamiltonian();

    virtual void print_header() const;
    virtual boost::shared_ptr<Vector> diagonal();
    virtual void product(const std::vector<boost::shared_ptr<Vector> >& x,
                               std::vector<boost::shared_ptr<Vector> >& b);

};

class MatrixUHamiltonian : public UHamiltonian {

protected:

    std::pair<boost::shared_ptr<Matrix>, boost::shared_ptr<Matrix> > M_;

public:

    MatrixUHamiltonian(std::pair<boost::shared_ptr<Matrix>, boost::shared_ptr<Matrix> > M);
    virtual ~MatrixUHamiltonian();

    virtual void print_header() const;
    virtual std::pair<boost::shared_ptr<Vector>,
                      boost::shared_ptr<Vector> > diagonal();
    virtual void product(const std::vector<std::pair<boost::shared_ptr<Vector>, boost::shared_ptr<Vector> > >& x,
                               std::vector<std::pair<boost::shared_ptr<Vector>, boost::shared_ptr<Vector> > >& b);

};

class CISRHamiltonian : public RHamiltonian {

protected:
    
    bool singlet_;
    boost::shared_ptr<Matrix> Caocc_;
    boost::shared_ptr<Matrix> Cavir_;
    boost::shared_ptr<Vector> eps_aocc_;
    boost::shared_ptr<Vector> eps_avir_;

public:
    CISRHamiltonian(boost::shared_ptr<JK> jk, 
                    boost::shared_ptr<Matrix> Caocc, 
                    boost::shared_ptr<Matrix> Cavir, 
                    boost::shared_ptr<Vector> eps_aocc,
                    boost::shared_ptr<Vector> eps_avir);
    virtual ~CISRHamiltonian();

    virtual void print_header() const;
    virtual boost::shared_ptr<Vector> diagonal();
    virtual void product(const std::vector<boost::shared_ptr<Vector> >& x,
                               std::vector<boost::shared_ptr<Vector> >& b);

    void set_singlet(bool singlet) { singlet_ = singlet; }
};

class CPHFRHamiltonian : public RHamiltonian {

protected:
    
    boost::shared_ptr<Matrix> Caocc_;
    boost::shared_ptr<Matrix> Cavir_;
    boost::shared_ptr<Vector> eps_aocc_;
    boost::shared_ptr<Vector> eps_avir_;

public:
    CPHFRHamiltonian(boost::shared_ptr<JK> jk, 
                     boost::shared_ptr<Matrix> Caocc, 
                     boost::shared_ptr<Matrix> Cavir, 
                     boost::shared_ptr<Vector> eps_aocc,
                     boost::shared_ptr<Vector> eps_avir);
    virtual ~CPHFRHamiltonian();

    virtual void print_header() const;
    virtual boost::shared_ptr<Vector> diagonal();
    virtual void product(const std::vector<boost::shared_ptr<Vector> >& x,
                               std::vector<boost::shared_ptr<Vector> >& b);

};

}
#endif