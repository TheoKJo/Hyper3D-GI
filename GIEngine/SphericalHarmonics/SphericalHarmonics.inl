template<unsigned int order>
void SphericalHarmonics<order>::Reset()
{
	for( unsigned int i = 0; i < GetNumberOfCoefficients(); i++ )
		coefficients[i] = 0.0f;
}

template<unsigned int order>
float SphericalHarmonics<order>::DotProduct( const SphericalHarmonics &SH ) const
{
	unsigned int NumberOfCoefficients = GetNumberOfCoefficients()<SH.GetNumberOfCoefficients()?GetNumberOfCoefficients():SH.GetNumberOfCoefficients();

	float dot = 0.0f;
	for( unsigned int i = 0; i < NumberOfCoefficients; i++ )
		dot += coefficients[i] * SH.coefficients[i];
	return dot;
}

//template<unsigned int order, unsigned int order2>
//float SphericalHarmonics<order>::DotProduct( const SphericalHarmonics<order2> &SH ) const
//{
//	unsigned int NumberOfCoefficients = GetNumberOfCoefficients()<SH.GetNumberOfCoefficients()?GetNumberOfCoefficients():SH.GetNumberOfCoefficients();
//
//	float dot = 0.0f;
//	for( unsigned int i = 0; i < NumberOfCoefficients; i++ )
//		dot += coefficients[i] * SH.coefficients[i];
//	return dot;
//}

// component-wise operator
template<unsigned int order>
SphericalHarmonics<order> SphericalHarmonics<order>::operator+( SphericalHarmonics<order> sh ) const
{
	SphericalHarmonics<order> ret;
	for( unsigned int i = 0; i < GetNumberOfCoefficients(); i++ )
		ret.coefficients[i] = this->coefficients[i] + sh.coefficients[i];
	return ret;
}

template<unsigned int order>
SphericalHarmonics<order> SphericalHarmonics<order>::operator+=( SphericalHarmonics<order> sh )
{
	for( unsigned int i = 0; i < GetNumberOfCoefficients(); i++ )
		this->coefficients[i] += sh.coefficients[i];
	return *this;
}

template<unsigned int order>
SphericalHarmonics<order> SphericalHarmonics<order>::operator*( float scalar ) const
{
	SphericalHarmonics<order> ret;
	for( unsigned int i = 0; i < GetNumberOfCoefficients(); i++ )
		ret.coefficients[i] = this->coefficients[i] * scalar;
	return ret;
}

inline int factorial( int n )
{
	if( n == 0 || n == 1 )
		return 1;
	return factorial( n-1 ) * n;
}

inline float K( int l, int m )
{
	assert( m >= 0 ); //!< 원래는 m을 받아, |m|으로 계산하지만, 여기서는 |m|이 들어온다고 가정함.

	float temp = (2.0f*l+1.0f) * factorial( l - m );
	temp = temp/( 4.0f * FLOAT_PI * factorial( l + m ) );
	return sqrt( temp );
}

inline float P( int l, int m, float x )
{
	float pmm = 1.0f;
	if( m > 0 )
	{
		//float somx2 = sqrt( (1.0f-x)*(1.0f+x) );
		float somx2 = sqrt( 1.0f-x*x );
		float fact = 1.0f;
		for( int i = 1; i <= m; i++ )
		{
			pmm *= (-fact) * somx2;
			fact += 2.0f;
		}
	}
	if( l == m )
		return pmm;
	float pmmp1 = x * (2.0f*m+1.0f)*pmm; //!< p_{m+1}^m
	if( l == m+1 )
		return pmmp1;
	float pll = 0.0f;		// !< p_{m+2}^m
	float pll1 = pmmp1;		// !< p_{m+1}^m
	float pll2 = pmm;		// !< p_{m}^m
	for( int ll = m+2; ll <= l; ll++ ) // m과 l을 동시에 하나씩 올린다고 보면 됨
	{
		pll = ( (2.0f*ll-1.0f) * x * pll1 - (ll+m-1.0f)*pll2 ) / (ll-m);
		pll2 = pll1;
		pll1 = pll;
	}
	return pll;
}

template<unsigned int order>
float SphericalHarmonics<order>::SH( int l, int m, float theta, float phi )
{
	assert( 0.0f <= theta && theta <= FLOAT_PI + 1e-5f );
	assert( 0.0f <= phi && phi < 2.0f * FLOAT_PI + 1e-5f );

	float sh = -1.0f;
	// reference: Spherical Harmonics Lighting by Robert Green
	if( m == 0 )
		sh = K( l, 0 ) * P( l, 0, cos( theta) );
	else if( m > 0 )
		sh = sqrt( 2.0f ) * K( l, m ) * cos( m * phi ) * P( l, m, cos( theta ) );
	else
		sh = sqrt( 2.0f ) * K( l, -m ) * sin( -m * phi ) * P( l, -m, cos( theta ) );
	return sh;
}

template<unsigned int order>
SphericalHarmonicsRGB<order> SphericalHarmonicsRGB<order>::operator+( SphericalHarmonicsRGB<order> sh ) const
{
	SphericalHarmonics<order> r, g, b;
	for( unsigned int i = 0; i < GetNumberOfCoefficients(); i++ )
	{
		r.coefficients[i] = this->r.coefficients[i] + sh.r.coefficients[i];
		g.coefficients[i] = this->g.coefficients[i] + sh.g.coefficients[i];
		b.coefficients[i] = this->b.coefficients[i] + sh.b.coefficients[i];
	}
	return SphericalHarmonicsRGB<order>( r, g, b );
}

template<unsigned int order>
SphericalHarmonicsRGB<order> SphericalHarmonicsRGB<order>::operator+=( SphericalHarmonicsRGB<order> sh )
{
	for( unsigned int i = 0; i < GetNumberOfCoefficients(); i++ )
	{
		this->r.coefficients[i] += sh.r.coefficients[i];
		this->g.coefficients[i] += sh.g.coefficients[i];
		this->b.coefficients[i] += sh.b.coefficients[i];
	}
	return *this;
}

template<unsigned int order>
SphericalHarmonicsRGB<order> SphericalHarmonicsRGB<order>::operator*( float scalar ) const
{
	SphericalHarmonics<order> r, g, b;
	for( unsigned int i = 0; i < GetNumberOfCoefficients(); i++ )
	{
		r.coefficients[i] = this->r.coefficients[i] * scalar;
		g.coefficients[i] = this->g.coefficients[i] * scalar;
		b.coefficients[i] = this->b.coefficients[i] * scalar;
	}
	return SphericalHarmonicsRGB<order>( r, g, b );
}