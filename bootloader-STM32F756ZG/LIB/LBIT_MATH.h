#ifndef BIT_MATH_H
#define BIT_MATH_H

// Set a specific bit in a register
#define SET_BIT(REG, BIT)        ((REG) |= (1 << (BIT)))

// Clear a specific bit in a register
#define CLEAR_BIT(REG, BIT)      ((REG) &= ~(1 << (BIT)))

// Toggle a specific bit in a register
#define TOGGLE_BIT(REG, BIT)     ((REG) ^= (1 << (BIT)))

// Check if a specific bit is set in a register
#define IS_BIT_SET(REG, BIT)     (((REG) & (1 << (BIT))) >> (BIT))

// Check if a specific bit is cleared in a register
#define IS_BIT_CLEAR(REG, BIT)   (!(((REG) & (1 << (BIT))) >> (BIT)))

// Get the value of a specific bit in a register
#define GET_BIT(REG, BIT)        (((REG) >> (BIT)) & 1)

// Set a register with a specific value
#define WRITE_REG(REG, VAL)      ((REG) = (VAL))

// Read the value of a register
#define READ_REG(REG)            (REG)

// Set multiple bits in a register
#define SET_BITS(REG, MASK)      ((REG) |= (MASK))

// Clear multiple bits in a register
#define CLEAR_BITS(REG, MASK)    ((REG) &= ~(MASK))

// Toggle multiple bits in a register
#define TOGGLE_BITS(REG, MASK)   ((REG) ^= (MASK))

#endif // BIT_MATH_H
