яю# i n c l u d e   " s t d a f x . h "  
 # i n c l u d e   " m a t h . h "  
 # i n c l u d e   " w i n d o w U n i M o d . h "  
  
 i n t   ( _ _ c d e c l   * n o x G u i F o n t H e i g h t M B )   ( v o i d *   F o n t P t r ) ;  
 i n t   ( _ _ c d e c l   * n o x S u b _ 4 3 F 6 7 0 )   ( i n t   F l a g ) ;   / /   =5  C25@5= 
 i n t   ( _ _ c d e c l   * n o x D r a w G e t S t r i n g S i z e )   ( v o i d   * F o n t P t r ,   c o n s t   w c h a r _ t * S t r i n g , i n t   * W i d t h , i n t   * H , i n t ) ;  
 i n t   ( _ _ c d e c l   * n o x S e t R e c t C o l o r M B )   ( i n t ) ;  
 i n t   ( _ _ c d e c l   * n o x D r a w R e c t )   ( i n t   x L e f t , i n t   y T o p , i n t   w i d t h , i n t   h e i g h t ) ;  
 i n t   ( _ _ c d e c l   * n o x D r a w R e c t A l p h a )   ( i n t   x L e f t , i n t   y T o p , i n t   w i d t h , i n t   h e i g h t ) ;  
 v o i d   * ( _ _ c d e c l   * g u i F o n t P t r B y N a m e ) ( c o n s t   c h a r   * F o n t N a m e ) ;  
  
 i n t   ( _ _ c d e c l   * n o x D r a w L i s t B o x W i t h I m a g e )   ( v o i d   * W i n d o w , v o i d   * W d d ) ;  
 i n t   ( _ _ c d e c l   * n o x D r a w L i s t B o x N o I m a g e )   ( v o i d   * W i n d o w , v o i d   * W d d ) ;  
  
 v o i d   ( _ _ c d e c l   * s c r e e n G e t S i z e ) ( i n t   & X , i n t   & Y ) ;  
  
 v o i d   ( _ _ c d e c l   * n o x R a s t e r P o i n t ) ( i n t   X , i n t   Y ) ;  
 v o i d   ( _ _ c d e c l   * n o x R a s t e r P o i n t R e l ) ( i n t   X , i n t   Y ) ;  
 v o i d   ( _ _ c d e c l   * n o x R a s t e r D r a w L i n e s ) ( ) ;  
 v o i d   ( _ _ c d e c l   * n o x G u i D r a w C u r s o r )   ( ) ;  
  
 e x t e r n   v o i d   ( _ _ c d e c l   * d r a w S e t T e x t C o l o r ) ( D W O R D   C o l o r ) ;  
 e x t e r n   v o i d   ( _ _ c d e c l   * d r a w S t r i n g ) ( v o i d   * F o n t P t r , c o n s t   w c h a r _ t * S t r i n g , i n t   X , i n t   Y ) ;  
 e x t e r n   v o i d   * ( _ _ c d e c l   * g L o a d I m g ) ( c o n s t   c h a r   * N a m e ) ;  
 e x t e r n   D W O R D   p a r s e C o l o r ( c o n s t   c h a r   * C o l o r ) ;  
 e x t e r n   v o i d   ( _ _ c d e c l   * d r a w I m a g e ) ( v o i d   * I m g H , i n t   X , i n t   Y ) ;  
  
 c o n s t   i n t   * n o x S c r e e n X = 	 	 ( i n t * ) 0 x 0 0 6 9 A 5 E 0 ;  
 c o n s t   i n t   * n o x S c r e e n Y = 	 	 ( i n t * ) 0 x 0 0 6 9 A 5 E 4 ;  
 c o n s t   i n t   * n o x S c r e e n W i d t h = 	 ( i n t * ) 0 x 0 0 6 9 A 5 F 0 ;  
 c o n s t   i n t   * n o x S c r e e n H i e g h t = 	 ( i n t * ) 0 x 0 0 6 9 A 5 F 4 ;  
 c o n s t   i n t   * n o x D r a w X L e f t = 	 ( i n t * ) 0 x 0 0 6 9 A 5 D 0 ;  
 c o n s t   i n t   * n o x D r a w Y T o p = 	 	 ( i n t * ) 0 x 0 0 6 9 A 5 D 4 ;  
  
 i n t   * c u r s o r S c r e e n X = ( i n t * ) 0 x 0 0 6 9 9 0 B 0 ;  
 i n t   * c u r s o r S c r e e n Y = ( i n t * ) 0 x 0 0 6 9 9 0 B 4 ;  
  
 D W O R D   p a r s e C o l o r ( c o n s t   c h a r   * C o l o r )  
 {  
 	 D W O R D   R e t = 0 , R = 0 , G = 0 , B = 0 ;  
 	 c o n s t   c h a r   * P = C o l o r ;  
 	 i f   ( C o l o r = = N U L L   | |   s t r c m p i ( C o l o r , " T R A N S P A R E N T " ) = = 0 )  
 	 	 r e t u r n   0 x 8 0 0 0 0 0 0 0 ;  
 	 i f   ( * P ! = ' # ' )  
 	 	 r e t u r n   R e t ;  
 	 P + + ;  
 	 f o r   ( i n t   i = 1 ; * P ! = 0 ; P + + , i + + )  
 	 {  
 	 	 R e t < < = 4 ;  
 	 	 i f   ( * P > = ' 0 '   & &   * P < = ' 9 ' )  
 	 	 	 R e t + = ( * P ) - ' 0 ' ;  
 	 	 e l s e   i f   ( * P > = ' A '   & &   * P < = ' F ' )  
 	 	 	 R e t + = ( * P ) - ' A ' + 1 0 ;  
 	 	 i f   ( i = = 2 )  
 	 	 {  
 	 	 	 R = R e t ; R e t = 0 ;  
 	 	 } e l s e   i f ( i = = 4 )  
 	 	 {  
 	 	 	 G = R e t ; R e t = 0 ;  
 	 	 } e l s e   i f ( i = = 6 )  
 	 	 {  
 	 	 	 B = R e t ; R e t = 0 ;  
 	 	 }  
 	 }  
 	 R e t = ( R > > 3 ) < < 1 1   |   ( G > > 2 ) < < 5   | ( B > > 3 ) ; / / / 4>;10=K9  1 6 18B=K9  F25B 
 	 R e t | = R e t < < 1 6 ;  
 	 r e t u r n   R e t ;  
 }  
 v o i d   w s t r i n g F r o m L u a ( l u a _ S t a t e   * L , w c h a r _ t   * D s t , i n t   M a x L e n )  
 {  
 	 s i z e _ t   L e n = 0 ;  
 	 c o n s t   c h a r   * S r c = l u a _ t o l s t r i n g ( L , - 1 , & L e n ) ;  
 	 i f   ( S r c = = N U L L )  
 	 {  
 	 	 w c s c p y ( D s t , L " ( n u l l ) " ) ;  
 	 	 r e t u r n ;  
 	 }  
 	 i f   ( L e n > M a x L e n )  
 	 {  
 	 	 w c s c p y ( D s t , L " ( l o n g ) " ) ;  
 	 	 r e t u r n ;  
 	 }  
 	 m b s t o w c s ( D s t , S r c , M a x L e n ) ;  
 }  
 v o i d   * i m a g e F r o m L u a ( l u a _ S t a t e   * L )  
 {  
 	 v o i d   * R e t = N U L L ;  
 	 i f   ( l u a _ t y p e ( L , - 1 ) = = L U A _ T S T R I N G )  
 	 {  
 	 	 R e t = g L o a d I m g ( l u a _ t o s t r i n g ( L , - 1 ) ) ;  
 	 } e l s e   i f   ( l u a _ t y p e ( L , - 1 ) = = L U A _ T L I G H T U S E R D A T A )  
 	 {  
 	 	 R e t = l u a _ t o u s e r d a t a ( L , - 1 ) ;  
 	 }  
 	 r e t u r n   R e t ;  
 }  
 v o i d   p a r s e A t t r ( l u a _ S t a t e   * L , i n t   K e y I d x , i n t   V a l I d x , v o i d   * S t r u c t , c o n s t   P a r s e A t t r i b   * A t t r s )  
 {  
 	 b y t e   * H = ( b y t e   * ) S t r u c t ;  
 	 c o n s t   c h a r   * K e y = l u a _ t o s t r i n g ( L , K e y I d x ) ;  
 	 s i z e _ t   L e n ;  
 	 c o n s t   c h a r   * V a l = l u a _ t o l s t r i n g ( L , V a l I d x , & L e n ) ;  
 	 f o r   ( P a r s e A t t r i b   c o n s t   * P = A t t r s ; P - > n a m e ! = N U L L ; P + + )  
 	 {  
 	 	 i f   ( 0 ! = s t r c m p ( P - > n a m e , K e y ) )  
 	 	 	 c o n t i n u e ;  
 	 	 s w i t c h   ( P - > t y p e )  
 	 	 {  
 	 	 c a s e   2 :   / / s t r i n g  
 	 	 	 {  
 	 	 	 c h a r   * N e w S t r i n g = ( c h a r * ) n o x A l l o c ( L e n + 1 ) ;  
 	 	 	 s t r c p y ( N e w S t r i n g , V a l ) ;  
 	 	 	 * ( ( c h a r * * ) (   H   +   P - > o f s ) ) = N e w S t r i n g ;  
 	 	 	 }  
 	 	 	 b r e a k ;  
 	 	 c a s e   3 :   / / c o l o r  
 	 	 	 * ( ( D W O R D * ) (   H   +   P - > o f s ) ) = p a r s e C o l o r ( V a l ) ;  
 	 	 	 b r e a k ;  
 	 	 c a s e   4 : / /   w s t r i n g  
 	 	 	 {  
 	 	 	 	 w c h a r _ t   * N e w S t r i n g = ( w c h a r _ t   * ) n o x A l l o c ( 2 * ( L e n + 1 ) ) ;  
 	 	 	 	 m b s t o w c s ( N e w S t r i n g , V a l , L e n + 1 ) ;  
 	 	 	 	 * ( ( w c h a r _ t * * ) (   H   +   P - > o f s ) ) = N e w S t r i n g ;  
 	 	 	 }  
 	 	 	 b r e a k ;  
 	 	 c a s e   5 :   / / b i t f i e l d  
 	 	 	 b r e a k ; / / T O D O  
 	 	 }  
 	 }  
 }  
  
 	 i n t   _ _ c d e c l   u n i W i n d o w D r a w F n ( v o i d   * W i n d o w , v o i d   * W i n d o w D D )   / /   !?5F80;L=0O  D=  @8A>20=8O,   GB>  1K  1K;0  8  ?@>7@0G=>ABL 
 	 {  
 	 	 B Y T E   * W n d = ( B Y T E * )   W i n d o w ;  
 	 	 w d d C o n t r o l   * W d d = ( w d d C o n t r o l * )   W i n d o w D D ;  
 	 	 i n t   x ;   i n t   y ;  
 	 	 i n t   * p x = & x ;   i n t   * p y = & y ;  
 	 	 n o x W n d G e t P o s t i o n ( W i n d o w , p x , p y ) ;  
 	 	 i f   ( ( * ( i n t * ) ( W n d + 4 ) )   &   0 x 8 0 )   / /   5A;8  5ABL  MB>B  D;03  = >   @8AC5<  :0@B8=:C 
 	 	 {  
 	 	 	 x + = W d d - > o f f s e t X ;   y + = W d d - > o f f s e t Y ;  
 	 	 	 i f   ( W d d - > f l a g s 0   &   2 )  
 	 	 	 	 d r a w I m a g e ( W d d - > i m a g e H i l i t e H , x , y ) ;  
 	 	 	 e l s e  
 	 	 	 	 d r a w I m a g e ( W d d - > i m a g e H , x , y ) ;  
 	 	 	 r e t u r n   1 ;  
 	 	 }  
 	 	 i f   ( W d d - > B g C o l o r = = 0 x 8 0 0 0 0 0 0 0 )  
 	 	 	 r e t u r n   1 ;   / /  
 	 	 n o x S e t R e c t C o l o r M B ( W d d - > B g C o l o r ) ;  
 	 	 i n t   T o p = l u a _ g e t t o p ( L ) ;  
 	 	 l u a _ p u s h l i g h t u s e r d a t a ( L , & n o x W n d L o a d ) ;  
 	 	 l u a _ g e t t a b l e ( L , L U A _ R E G I S T R Y I N D E X ) ;  
 	 	 l u a _ p u s h l i g h t u s e r d a t a ( L , W i n d o w ) ;  
 	 	 l u a _ g e t t a b l e ( L , - 2 ) ;  
 	 	 i f   ( l u a _ t y p e ( L , - 1 ) ! = L U A _ T T A B L E )  
 	 	 {  
 	 	 	 n o x D r a w R e c t ( x , y , * ( ( i n t * ) ( W n d + 8 ) ) , * ( ( i n t * ) ( W n d + 0 x C ) ) ) ;  
 	 	 	 r e t u r n   1 ;  
 	 	 }  
 	 	 l u a _ g e t f i e l d ( L , - 1 , " a l p h a " ) ;  
 	 	 i f   ( l u a _ t o b o o l e a n ( L , - 1 ) )  
 	 	 	 n o x D r a w R e c t A l p h a ( x , y , * ( ( i n t * ) ( W n d + 8 ) ) , * ( ( i n t * ) ( W n d + 0 x C ) ) ) ;  
 	 	 e l s e  
 	 	 	 n o x D r a w R e c t ( x , y , * ( ( i n t * ) ( W n d + 8 ) ) , * ( ( i n t * ) ( W n d + 0 x C ) ) ) ;  
 	 	 l u a _ s e t t o p ( L , T o p ) ;  
 	 	 r e t u r n   1 ;  
 	 }  
  
 	 i n t   _ _ c d e c l   u n i L i s t B o x D r a w F n ( w n d S t r u c t   * W i n d o w , v o i d   * W i n d o w D D )  
 	 {  
 	 	 w n d S t r u c t   * W n d C h i l d = 0 ;  
 	 	 l i s t B o x D a t a S t r u c t   * L i s t b o x D a t a = ( l i s t B o x D a t a S t r u c t * ) W i n d o w - > s o m e D a t a ;  
 	 	 i n t   r e t = 0 ;  
 	 	 i f   ( L i s t b o x D a t a - > s l i d e r ! = 0 )  
 	 	 	 W n d C h i l d = L i s t b o x D a t a - > s l i d e r ;  
 	 	 e l s e   i f   ( L i s t b o x D a t a - > b u t t o n U p ! = 0 )  
 	 	 	 W n d C h i l d = L i s t b o x D a t a - > b u t t o n U p ;  
 	 	 e l s e    
 	 	 	 W n d C h i l d = L i s t b o x D a t a - > b u t t o n D o w n ; 	 	  
 	 	 i n t   w i d t h = W i n d o w - > w i d t h ;  
 	 	 i f   ( W n d C h i l d ! = 0 )  
 	 	 {  
 	 	 	 i f   ( ! ( W n d C h i l d - > s c r e e n L e f t > W i n d o w - > s c r e e n R i g h t )   & &   ! ( W n d C h i l d - > s c r e e n L e f t < W i n d o w - > s c r e e n L e f t ) )  
 	 	 	 	 W i n d o w - > w i d t h = w i d t h - ( W i n d o w - > s c r e e n R i g h t - W n d C h i l d - > s c r e e n R i g h t ) ;  
 	 	 }  
 	 	 i f   ( ( W i n d o w - > f l a g s   &   w f I m a g e ) = = 0 )  
 	 	 	 r e t = n o x D r a w L i s t B o x N o I m a g e ( W i n d o w , W i n d o w D D ) ;  
 	 	 e l s e    
 	 	 	 r e t = n o x D r a w L i s t B o x W i t h I m a g e ( W i n d o w , W i n d o w D D ) ;  
 	 	 W i n d o w - > w i d t h = w i d t h ;  
 	 	 r e t u r n   r e t ;  
 	 }  
  
 n a m e s p a c e    
 {  
  
 	 i n t   s t r i n g G e t S i z e L ( l u a _ S t a t e * L )  
 	 {  
 	 	 i f   ( l u a _ t y p e ( L , 1 ) ! = L U A _ T S T R I N G )  
 	 	 {  
 	 	 	 l u a _ p u s h s t r i n g ( L , " w r o n g   a r g s " ) ;  
 	 	 	 l u a _ e r r o r _ ( L ) ;  
 	 	 }  
 	 	 c o n s t   c h a r   * V = l u a _ t o s t r i n g ( L , 1 ) ;  
 	 	 w c h a r _ t   W [ 5 0 0 ] ;  
 	 	 m b s t o w c s ( W ,   V , s t r l e n ( V ) ) ;  
 	 	 W [ s t r l e n ( V ) ] = 0 ;  
 	 	 w c h a r _ t   * p W = W ;  
 	 	 i n t   W i d t h = 0 ;  
 	 	 i n t   H e i g h t = 0 ;  
 	 	 n o x D r a w G e t S t r i n g S i z e ( N U L L , p W , & W i d t h , & H e i g h t , 0 ) ;  
 	 	 l u a _ p u s h n u m b e r ( L , W i d t h ) ;  
 	 	 l u a _ p u s h n u m b e r ( L , H e i g h t ) ;  
 	 	 r e t u r n   2 ;  
 	 }  
  
  
 	 i n t   s t r i n g D r a w L ( l u a _ S t a t e * L )  
 	 {  
 	 	 i f   ( l u a _ t y p e ( L , 1 ) ! = L U A _ T S T R I N G   | |   l u a _ t y p e ( L , 2 ) ! = L U A _ T N U M B E R   | |   l u a _ t y p e ( L , 3 ) ! = L U A _ T N U M B E R )  
 	 	 {  
 	 	 	 l u a _ p u s h s t r i n g ( L , " w r o n g   a r g s " ) ;  
 	 	 	 l u a _ e r r o r _ ( L ) ;  
 	 	 }  
 	 	 D W O R D   c o l o r = 0 x F F F F F F ;  
 	 	 i f   ( l u a _ t y p e ( L , 4 ) = = L U A _ T S T R I N G )  
 	 	 	 c o l o r = ( i n t ) p a r s e C o l o r ( l u a _ t o s t r i n g ( L , 4 ) ) ;  
 	 	 c o n s t   c h a r   * V = l u a _ t o s t r i n g ( L , 1 ) ;  
 	 	 w c h a r _ t   W [ 5 0 0 ] ;  
 	 	 m b s t o w c s ( W ,   V , s t r l e n ( V ) ) ;  
 	 	 W [ s t r l e n ( V ) ] = 0 ;  
 	 	 d r a w S e t T e x t C o l o r ( c o l o r ) ;  
 	 	 d r a w S t r i n g ( N U L L , W , l u a _ t o n u m b e r ( L , 2 ) , l u a _ t o n u m b e r ( L , 3 ) ) ;  
 	 	 r e t u r n   0 ;  
 	 }  
  
 	 i n t   s c r e e n G e t P o s L ( l u a _ S t a t e * L )  
 	 {  
 	 	 l u a _ p u s h i n t e g e r ( L , * n o x S c r e e n X ) ;  
 	 	 l u a _ p u s h i n t e g e r ( L , * n o x S c r e e n Y ) ;  
 	 	 r e t u r n   2 ;  
 	 }  
  
 	 i n t   s c r e e n G e t S i z e L ( l u a _ S t a t e * L )  
 	 {  
 	 	 l u a _ p u s h i n t e g e r ( L , * n o x S c r e e n W i d t h ) ;  
 	 	 l u a _ p u s h i n t e g e r ( L , * n o x S c r e e n H i e g h t ) ;  
 	 	 r e t u r n   2 ;  
 	 }  
 	 i n t   p o s W o r l d T o S c r e e n L ( l u a _ S t a t e * L )  
 	 {  
 	 	 i f   ( l u a _ t y p e ( L , 1 ) ! = L U A _ T N U M B E R   | |   l u a _ t y p e ( L , 2 ) ! = L U A _ T N U M B E R )  
 	 	 {  
 	 	 	 l u a _ p u s h s t r i n g ( L , " w r o n g   a r g s ! " ) ;  
 	 	 	 l u a _ e r r o r _ ( L ) ;  
 	 	 }  
 	 	 f l o a t   x 1 = l u a _ t o n u m b e r ( L , 1 ) - * n o x S c r e e n X ;  
 	 	 x 1 + = * n o x D r a w X L e f t ;  
 	 	 f l o a t   y 1 = * n o x D r a w Y T o p - * n o x S c r e e n Y ;  
 	 	 y 1 + = l u a _ t o n u m b e r ( L , 2 ) ;  
 	 	 l u a _ p u s h i n t e g e r ( L , x 1 ) ;  
 	 	 l u a _ p u s h i n t e g e r ( L , y 1 ) ;  
 	 	 r e t u r n   2 ;  
 	 }  
  
 	 i n t   c l i P l a y e r M o u s e L ( l u a _ S t a t e   * L )  
 	 {  
 	 	 i f   ( l u a _ t y p e ( L , 1 ) = = L U A _ T N U M B E R   & &   l u a _ t y p e ( L , 2 ) = = L U A _ T N U M B E R )  
 	 	 {  
 	 	 	 i n t   x 1 = l u a _ t o i n t e g e r ( L , 1 ) ;  
 	 	 	 i n t   y 1 = l u a _ t o i n t e g e r ( L , 2 ) ;  
 	 	 	 i f   ( ( x 1 > 0   & &   x 1 < * n o x S c r e e n W i d t h )   & &   ( y 1 > 0   & &   y 1 < * n o x S c r e e n H i e g h t ) )  
 	 	 	 {  
 	 	 	 	 * c u r s o r S c r e e n X = x 1 ;  
 	 	 	 	 * c u r s o r S c r e e n Y = y 1 ;  
 	 	 	 }  
 	 	 }  
 	 	 e l s e  
 	 	 {  
 	 	 	 i n t   x = * c u r s o r S c r e e n X + * n o x S c r e e n X ;  
 	 	 	 x - = * n o x D r a w X L e f t ;  
 	 	 	 i n t   y = * n o x D r a w Y T o p - * n o x S c r e e n Y ;  
 	 	 	 y - = * c u r s o r S c r e e n Y ;  
 	 	 	 l u a _ p u s h n u m b e r ( L , x ) ;  
 	 	 	 l u a _ p u s h n u m b e r ( L , y * - 1 ) ;  
 	 	 	 r e t u r n   2 ;  
 	 	 }  
 	 	 r e t u r n   0 ;  
 	 }  
 	 i n t   d r a w L i n e s N o w ( l u a _ S t a t e   * L )  
 	 {  
 	 	 i n t   n = l u a _ g e t t o p ( L ) ;  
 	 	 i f   ( n < 3   | |   l u a _ t y p e ( L , 1 ) ! = L U A _ T S T R I N G )  
 	 	 {  
 	 	 	 l u a _ p u s h s t r i n g ( L , " w r o n g   a r g s ! " ) ;  
 	 	 	 l u a _ e r r o r _ ( L ) ;  
 	 	 }  
 	 	 n o x S e t R e c t C o l o r M B ( p a r s e C o l o r ( l u a _ t o s t r i n g ( L , 1 ) ) ) ;  
 	 	 f o r   ( i n t   i = 2 ; i < n ; i + = 2 )  
 	 	 {  
 	 	 	 n o x R a s t e r P o i n t ( l u a _ t o i n t e g e r ( L , i ) , l u a _ t o i n t e g e r ( L , i + 1 ) ) ;  
 	 	 }  
 	 	 n o x R a s t e r D r a w L i n e s ( ) ;  
 	 	 r e t u r n   0 ;  
 	 }  
 	 i n t   d r a w F l o o r L i n e s ( l u a _ S t a t e   * L )  
 	 {  
 	 	 i n t   n = l u a _ g e t t o p ( L ) ;  
 	 	 i f   ( n < 3   | |   l u a _ t y p e ( L , 1 ) ! = L U A _ T S T R I N G )  
 	 	 {  
 	 	 	 l u a _ p u s h s t r i n g ( L , " w r o n g   a r g s ! " ) ;  
 	 	 	 l u a _ e r r o r _ ( L ) ;  
 	 	 }  
 	 	 i n t   I d = 0 ;  
 	 	 F x B u f f e r _ t   *   B   =   F x B u f f e r _ t : : a d d B l o c k ( n + 6 , & I d ) ;  
 	 	 B - > a d d I t e m ( 1 1 ) ;  
 	 	 B - > a d d I t e m ( 8 ) ;  
 	 	 B - > a d d I t e m D ( p a r s e C o l o r ( l u a _ t o s t r i n g ( L , 1 ) ) ) ;  
 	 	 B - > a d d I t e m ( 9 ) ;  
 	 	 B - > a d d I t e m ( n - 1 ) ;  
 	 	 f o r   ( i n t   i = 2 ; i < n ; i + = 2 )  
 	 	 {  
 	 	 	 i n t   X = l u a _ t o i n t e g e r ( L , i ) ;  
 	 	 	 i n t   Y = l u a _ t o i n t e g e r ( L , i + 1 ) ;  
 	 	 	 B - > a d d I t e m ( X ) ;  
 	 	 	 B - > a d d I t e m ( Y ) ;  
 	 	 }  
 	 	 B - > a d d I t e m ( 1 0 ) ;  
 	 	 l u a _ p u s h i n t e g e r ( L , I d ) ;  
 	 	 r e t u r n   1 ;  
 	 }  
 	 i n t   d r a w F l o o r T o C u r s o r ( l u a _ S t a t e   * L )  
 	 {  
 	 	 i n t   n = l u a _ g e t t o p ( L ) ;  
 	 	 i f   ( ( n ! = 3   & &   n ! = 4 )   | |   l u a _ t y p e ( L , 1 ) ! = L U A _ T S T R I N G )  
 	 	 {  
 	 	 	 l u a _ p u s h s t r i n g ( L , " w r o n g   a r g s ! " ) ;  
 	 	 	 l u a _ e r r o r _ ( L ) ;  
 	 	 }  
 	 	 l u a _ s e t t o p ( L , 4 ) ;  
 	 	 i n t   I d = 0 ;  
 	 	 F x B u f f e r _ t   *   B   =   F x B u f f e r _ t : : a d d B l o c k ( 8 , & I d ) ;  
 	 	 B - > a d d I t e m ( 1 1 ) ;  
 	 	 B - > a d d I t e m ( 8 ) ;  
 	 	 B - > a d d I t e m D ( p a r s e C o l o r ( l u a _ t o s t r i n g ( L , 1 ) ) ) ;  
 	 	 B - > a d d I t e m ( 1 4 ) ;  
 	 	 B - > a d d I t e m ( l u a _ t o i n t e g e r ( L , 2 ) ) ;  
 	 	 B - > a d d I t e m ( l u a _ t o i n t e g e r ( L , 3 ) ) ;  
 	 	 B - > a d d I t e m ( l u a _ i s n i l ( L , 4 ) ? 3 1 : l u a _ t o i n t e g e r ( L , 4 ) ) ;  
 	 	 B - > a d d I t e m ( 1 1 ) ;  
 	 	 l u a _ p u s h i n t e g e r ( L , I d ) ;  
 	 	 r e t u r n   1 ;  
 	 }  
 	 i n t   d r a w L i n e s ( l u a _ S t a t e   * L )  
 	 {  
 	 	 i n t   n = l u a _ g e t t o p ( L ) ;  
 	 	 i f   ( n < 3   | |   l u a _ t y p e ( L , 1 ) ! = L U A _ T S T R I N G )  
 	 	 {  
 	 	 	 l u a _ p u s h s t r i n g ( L , " w r o n g   a r g s ! " ) ;  
 	 	 	 l u a _ e r r o r _ ( L ) ;  
 	 	 }  
 	 	 i n t   I d = 0 ;  
 	 	 F x B u f f e r _ t   *   B   =   F x B u f f e r _ t : : a d d B l o c k ( n + 4 , & I d ) ;  
 	 	 B - > a d d I t e m ( 8 ) ;  
 	 	 B - > a d d I t e m D ( p a r s e C o l o r ( l u a _ t o s t r i n g ( L , 1 ) ) ) ;  
 	 	 B - > a d d I t e m ( 9 ) ;  
 	 	 B - > a d d I t e m ( n - 1 ) ;  
 	 	 f o r   ( i n t   i = 2 ; i < n ; i + = 2 )  
 	 	 {  
 	 	 	 B - > a d d I t e m ( l u a _ t o i n t e g e r ( L , i ) ) ;  
 	 	 	 B - > a d d I t e m ( l u a _ t o i n t e g e r ( L , i + 1 ) ) ;  
 	 	 }  
 	 	 l u a _ p u s h i n t e g e r ( L , I d ) ;  
 	 	 r e t u r n   1 ;  
 	 }  
 	 i n t   d e l L i n e s ( l u a _ S t a t e   * L )  
 	 {  
 	 	 i n t   I d = l u a _ t o n u m b e r ( L , 1 ) ;  
 	 	 F x B u f f e r _ t : : d e l B l o c k ( I d ) ;  
 	 	 r e t u r n   0 ;  
 	 }  
 	 i n t   _ _ d e c l s p e c ( n a k e d )   m y W n d D r a w A l l ( )  
 	 {  
 	 	 _ _ a s m  
 	 	 {  
 	 	 	 c a l l   F x B u f f e r _ t : : d r a w B u f f e r s  
 	 	 	 p u s h   0 x 0 0 4 6 C 2 E 0  
 	 	 	 r e t  
 	 	 }  
 	 }  
 }  
  
 e x t e r n   v o i d   I n j e c t A d d r ( D W O R D   A d d r , v o i d   * F n ) ;  
 e x t e r n   v o i d   I n j e c t O f f s ( D W O R D   A d d r , v o i d   * F n ) ;  
 e x t e r n   v o i d   I n j e c t J u m p T o ( D W O R D   A d d r , v o i d   * F n ) ;  
 e x t e r n   v o i d   I m a g e U t i l I n i t ( ) ;  
 v o i d   g u i I n i t ( )  
 {  
 	 I m a g e U t i l I n i t ( ) ;  
 	 A S S I G N ( n o x G u i D r a w C u r s o r , 0 x 0 0 4 7 7 8 3 0 ) ;  
 	 A S S I G N ( s c r e e n G e t S i z e , 0 x 0 0 4 3 0 C 5 0 ) ;  
 	 A S S I G N ( g u i F o n t P t r B y N a m e , 0 x 0 0 4 3 F 3 6 0 ) ;  
 	 A S S I G N ( n o x G u i F o n t H e i g h t M B , 0 x 0 0 4 3 F 3 2 0 ) ;  
 	 A S S I G N ( n o x S u b _ 4 3 F 6 7 0 , 0 x 4 3 F 6 7 0 ) ;  
 	 A S S I G N ( n o x D r a w G e t S t r i n g S i z e , 0 x 0 0 4 3 F 8 4 0 ) ;  
 	 A S S I G N ( n o x S e t R e c t C o l o r M B , 0 x 0 0 4 3 4 4 6 0 ) ;  
 	 A S S I G N ( n o x D r a w R e c t , 0 x 0 0 4 9 C E 3 0 ) ;  
 	 A S S I G N ( n o x D r a w R e c t A l p h a , 0 x 0 0 4 9 C F 1 0 ) ;  
  
 	 A S S I G N ( n o x D r a w L i s t B o x W i t h I m a g e , 0 x 0 0 4 A 3 F C 0 ) ;  
 	 A S S I G N ( n o x D r a w L i s t B o x N o I m a g e , 0 x 0 0 4 A 3 C 5 0 ) ;  
  
 	 A S S I G N ( n o x R a s t e r D r a w L i n e s , 0 x 0 0 4 9 E 4 B 0 ) ;  
 	 A S S I G N ( n o x R a s t e r P o i n t R e l , 0 x 0 0 4 9 F 5 7 0 ) ;  
 	 A S S I G N ( n o x R a s t e r P o i n t , 0 x 0 0 4 9 F 5 0 0 ) ;  
 	  
 	 I n j e c t O f f s ( 0 x 0 0 4 3 E 7 1 A + 1 , & m y W n d D r a w A l l ) ;  
 	  
 	 r e g i s t e r c l i e n t ( " c l i D r a w L i n e s " , & d r a w L i n e s ) ;  
 	 r e g i s t e r c l i e n t ( " c l i F l o o r L i n e s " , & d r a w F l o o r L i n e s ) ;  
 	 r e g i s t e r c l i e n t ( " c l i D e l L i n e s " , & d e l L i n e s ) ;  
 	 r e g i s t e r c l i e n t ( " c l i F l o o r T o C u r s o r " , & d r a w F l o o r T o C u r s o r ) ;  
 	  
 	  
 	 r e g i s t e r c l i e n t ( " c l i P l a y e r M o u s e " , & c l i P l a y e r M o u s e L ) ;  
 	 r e g i s t e r c l i e n t ( " s c r e e n G e t P o s " , s c r e e n G e t P o s L ) ;  
 	 r e g i s t e r c l i e n t ( " s c r e e n G e t S i z e " , s c r e e n G e t S i z e L ) ;  
 	 r e g i s t e r c l i e n t ( " p o s W o r l d T o S c r e e n " , p o s W o r l d T o S c r e e n L ) ;  
 	 r e g i s t e r c l i e n t ( " s t r i n g D r a w " , s t r i n g D r a w L ) ;  
 	 r e g i s t e r c l i e n t ( " s t r i n g G e t S i z e " , s t r i n g G e t S i z e L ) ;  
  
 } 